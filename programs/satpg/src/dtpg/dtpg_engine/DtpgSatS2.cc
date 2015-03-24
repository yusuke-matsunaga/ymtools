﻿
/// @file DtpgSatS2.cc
/// @brief DtpgSatS2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgSatS2.h"

#include "DtpgStats.h"
#include "TpgNetwork.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "SatEngine.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief Single2 エンジンを作る．
// @param[in] th_val しきい値
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] bt バックトレーサー
// @param[in] dop パタンが求められた時に実行されるファンクタ
// @param[in] uop 検出不能と判定された時に実行されるファンクタ
DtpgEngine*
new_DtpgSatS2(ymuint th_val,
	      const string& sat_type,
	      const string& sat_option,
	      ostream* sat_outp,
	      BackTracer& bt,
	      DetectOp& dop,
	      UntestOp& uop)
{
  return new DtpgSatS2(th_val, sat_type, sat_option, sat_outp, bt, dop, uop);
}

// @brief コンストラクタ
DtpgSatS2::DtpgSatS2(ymuint th_val,
		     const string& sat_type,
		     const string& sat_option,
		     ostream* sat_outp,
		     BackTracer& bt,
		     DetectOp& dop,
		     UntestOp& uop) :
  DtpgSatBaseS(sat_type, sat_option, sat_outp, bt, dop, uop),
  mThVal(th_val)
{
}

// @brief デストラクタ
DtpgSatS2::~DtpgSatS2()
{
}

// @brief テストパタン生成を行なう．
void
DtpgSatS2::run_single(TpgNetwork& network,
		      TpgFault* fault)
{
  TpgNode* fnode = fault->node();

  SatEngine engine(sat_type(), sat_option(), sat_outp());

  mMark.clear();
  mMark.resize(network.max_node_id(), 0);

  //////////////////////////////////////////////////////////////////////
  // 変数の割当
  //////////////////////////////////////////////////////////////////////
  for (ymuint i = 0; i < tfo_tfi_size(); ++ i) {
    TpgNode* node = tfo_tfi_node(i);
    VarId gvar = engine.new_var();
    node->set_gvar(gvar);
  }
  for (ymuint i = 0; i < tfo_size(); ++ i) {
    TpgNode* node = tfo_tfi_node(i);
    VarId fvar = engine.new_var();
    VarId dvar = engine.new_var();
    node->set_fvar(fvar, dvar);
  }

  const vector<TpgNode*>& olist = output_list();
  ymuint no = olist.size();

  ymuint th_val = mThVal;
  if ( th_val > no ) {
    th_val = no;
  }
  bool fault_remain = false;
  for (ymuint opos = 0; opos < th_val; ++ opos) {
    // onode に関係する部分の CNF を生成する．
    TpgNode* onode = olist[opos];
    ymuint oid = onode->output_id2();

    cnf_begin();

    //////////////////////////////////////////////////////////////////////
    // 正常回路の CNF を生成
    //////////////////////////////////////////////////////////////////////
    for (ymuint i = 0; i < tfo_tfi_size(); ++ i) {
      TpgNode* node = tfo_tfi_node(i);
      if ( node->is_in_TFI_of(oid) && mMark[node->id()] == 0 ) {
	mMark[node->id()] = opos + 1;

	make_gval_cnf(engine, node);
      }
    }

    //////////////////////////////////////////////////////////////////////
    // 故障回路の CNF を生成
    //////////////////////////////////////////////////////////////////////
    for (ymuint i = 0; i < tfo_size(); ++ i) {
      TpgNode* node = tfo_tfi_node(i);
      if ( mMark[node->id()] != (opos + 1) ) {
	continue;
      }

      // 故障回路のゲートの入出力関係を表すCNFを作る．
      if ( node == fnode ) {
	make_fault_cnf(engine, fault);
      }
      else {
	make_fval_cnf(engine, node);
      }

      make_dchain_cnf(engine, node);
    }

    // fnode の dvar は 1 でなければならない．
    {
      Literal dlit(fnode->dvar(), false);
      engine.add_clause(dlit);
    }

    cnf_end();

    timer_start();

    // まだ作っていない部分の dlit を 0 にする．
    engine.assumption_begin();
    for (ymuint i = 0; i < tfo_size(); ++ i) {
      TpgNode* node = tfo_tfi_node(i);
      if ( mMark[node->id()] == 0 ) {
	Literal dlit(node->dvar(), false);
	engine.assumption_add(~dlit);
      }
    }

    SatStats prev_stats;
    engine.get_stats(prev_stats);

    Bool3 ans = _solve(engine);

    SatStats sat_stats;
    engine.get_stats(sat_stats);

    sub_sat_stats(sat_stats, prev_stats);

    USTime time = timer_stop();

    if ( ans == kB3True ) {
      detect_op(fault, sat_stats, time);
      break;
    }
    else if ( ans == kB3False ) {
      if ( opos == no - 1 ) {
	untest_op(fault, sat_stats, time);
      }
      else {
	partially_untest_op(fault, sat_stats, time);
	fault_remain = true;
      }
    }
    else {
      abort_op(fault, sat_stats, time);
    }
  }

  if ( fault_remain ) {
    // 最後に全部の出力を対象にしてテスト生成を実行する．

    cnf_begin();

    //////////////////////////////////////////////////////////////////////
    // 正常回路の CNF を生成
    //////////////////////////////////////////////////////////////////////
    for (ymuint i = 0; i < tfo_tfi_size(); ++ i) {
      TpgNode* node = tfo_tfi_node(i);
      if ( mMark[node->id()] == 0 ) {
	make_gval_cnf(engine, node);
      }
    }

    //////////////////////////////////////////////////////////////////////
    // 故障回路の CNF を生成
    //////////////////////////////////////////////////////////////////////
    for (ymuint i = 0; i < tfo_size(); ++ i) {
      TpgNode* node = tfo_tfi_node(i);
      if ( mMark[node->id()] == 0 ) {
	make_fval_cnf(engine, node);
	make_dchain_cnf(engine, node);
      }
    }

    cnf_end();

    engine.assumption_begin();

    // dominator ノードの dvar は1でなければならない．
    for (TpgNode* node = fnode; node != NULL; node = node->imm_dom()) {
      engine.assumption_add(Literal(node->dvar(), false));
    }

    solve(engine, fault);
  }

  clear_node_mark();

  for (ymuint i = 0; i < tfo_tfi_size(); ++ i) {
    TpgNode* node = tfo_tfi_node(i);
    mMark[node->id()] = 0;
  }
}

END_NAMESPACE_YM_SATPG