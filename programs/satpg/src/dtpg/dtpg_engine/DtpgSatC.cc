﻿
/// @file DtpgSatC.cc
/// @brief DtpgSatC の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014, 2015 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgSatC.h"

#include "TpgNetwork.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "SatEngine.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief Concurrent エンジンを作る．
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] max_id ノード番号の最大値 + 1
// @param[in] bt バックトレーサー
// @param[in] dop パタンが求められた時に実行されるファンクタ
// @param[in] uop 検出不能と判定された時に実行されるファンクタ
DtpgEngine*
new_DtpgSatC(const string& sat_type,
	     const string& sat_option,
	     ostream* sat_outp,
	     BackTracer& bt,
	     DetectOp& dop,
	     UntestOp& uop)
{
  return new DtpgSatC(sat_type, sat_option, sat_outp, bt, dop, uop);
}

// @brief コンストラクタ
DtpgSatC::DtpgSatC(const string& sat_type,
		   const string& sat_option,
		   ostream* sat_outp,
		   BackTracer& bt,
		   DetectOp& dop,
		   UntestOp& uop) :
  DtpgSat(sat_type, sat_option, sat_outp, bt, dop, uop)
{
}

// @brief デストラクタ
DtpgSatC::~DtpgSatC()
{
}

// @brief テスト生成を行なう．
// @param[in] network 対象のネットワーク
// @param[in] stats 結果を格納する構造体
void
DtpgSatC::run(TpgNetwork& network,
	      DtpgStats& stats)
{
  clear_stats();

  mMark.clear();
  mMark.resize(network.max_node_id(), false);
  ymuint n = network.active_node_num();
  ymuint max_id = network.max_node_id();
  for (ymuint i = 0; i < n; ++ i) {
    TpgNode* node = network.active_node(i);
    if ( node->imm_dom() == NULL ) {
      mFaultNodeList.clear();
      mFaultList.clear();

      dfs_mffc(node);

      if ( !mFaultList.empty() ) {
	mark_region(max_id, vector<TpgNode*>(1, node));
	run_multi(network, node);
      }
    }
  }

  get_stats(stats);
}

// @brief DFS で MFFC を求める．
// @param[in] node 対象のノード
void
DtpgSatC::dfs_mffc(TpgNode* node)
{
  mMark[node->id()] = true;

  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    TpgNode* inode = node->fanin(i);
    if ( mMark[inode->id()] == false && inode->imm_dom() != NULL ) {
      dfs_mffc(inode);
    }
  }

  if ( !node->is_output() ) {
    mFaultNodeList.push_back(node);
    ymuint nf = node->fault_num();
    for (ymuint i = 0; i < nf; ++ i) {
      TpgFault* f = node->fault(i);
      if ( f->status() != kFsDetected &&
	   !f->is_skip() ) {
	mFaultList.push_back(f);
      }
    }
  }
}

// @brief 複数の故障を同時に検出するパタンを求める．
void
DtpgSatC::run_multi(TpgNetwork& network,
		    TpgNode* root)
{
  cnf_begin();

  SatEngine engine(sat_type(), sat_option(), sat_outp());

  ymuint nf = mFaultList.size();

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

  // 故障を活性化するとき true にする変数．
  vector<VarId> flt_var(nf);
  for (ymuint i = 0; i < nf; ++ i) {
    VarId fvar = engine.new_var();
    flt_var[i] = fvar;
    TpgFault* f = mFaultList[i];
    int fval = f->val();
    TpgNode* node = f->node();

    if ( f->is_output_fault() ) {
      node->set_ofvar(fval, fvar);
    }
    else {
      ymuint pos = f->pos();
      node->set_ifvar(pos, fval, fvar);
    }
  }

  //////////////////////////////////////////////////////////////////////
  // 正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  for (ymuint i = 0; i < tfo_tfi_size(); ++ i) {
    TpgNode* node = tfo_tfi_node(i);
    make_gval_cnf(engine, node);
  }

  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  for (ymuint i = 0; i < tfo_size(); ++ i) {
    TpgNode* node = tfo_tfi_node(i);
    if ( node != root ) {
      make_fval_cnf(engine, node);
    }
    make_dchain_cnf(engine, node);
  }

  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件
  //////////////////////////////////////////////////////////////////////
  ymuint npo = output_list().size();
  engine.tmp_lits_begin(npo);
  for (ymuint i = 0; i < npo; ++ i) {
    TpgNode* node = output_list()[i];
    Literal dlit(node->dvar(), false);
    engine.tmp_lits_add(dlit);
  }
  engine.tmp_lits_end();

  cnf_end();

  // 個々の故障に対するテスト生成を行なう．
  for (ymuint fid = 0; fid < nf; ++ fid) {
    TpgFault* f = mFaultList[fid];

    if ( f->status() == kFsDetected ) {
      // 他の故障のパタンで検出済みになっている場合がある．
      continue;
    }

    vector<Literal> assump_base;
    engine.assumption_begin();

    // 該当の故障に対する変数のみ1にする．
    for (ymuint j = 0; j < nf; ++ j) {
      bool inv = (j != fid);
      Literal flit(flt_var[j], inv);
      engine.assumption_add(flit);
      //assump_base.push_back(flit);
    }

    TpgNode* fnode = mFaultList[fid]->node();

    // 故障ノードの TFO 以外の dlit を0にする．
    mTmpNodeList.clear();
    mTmpNodeList.reserve(tfo_tfi_size());
    set_tmp_mark(fnode);
    mTmpNodeList.push_back(fnode);
    for (ymuint rpos = 0; rpos < mTmpNodeList.size(); ++ rpos) {
      TpgNode* node = mTmpNodeList[rpos];
      ymuint nfo = node->active_fanout_num();
      for (ymuint i = 0; i < nfo; ++ i) {
	TpgNode* fonode = node->active_fanout(i);
	if ( !tmp_mark(fonode) ) {
	  set_tmp_mark(fonode);
	  mTmpNodeList.push_back(fonode);
	}
      }
    }
    for (ymuint i = 0; i < tfo_tfi_size(); ++ i) {
      TpgNode* node = tfo_tfi_node(i);
      if ( node->has_fvar() && !tmp_mark(node) ) {
	Literal dlit(node->dvar(), true);
	engine.assumption_add(dlit);
	//assump_base.push_back(dlit);
      }
    }
    for (vector<TpgNode*>::iterator p = mTmpNodeList.begin();
	 p != mTmpNodeList.end(); ++ p) {
      TpgNode* node = *p;
      clear_tmp_mark(node);
    }
    mTmpNodeList.clear();

    // dominator ノードの dvar は1でなければならない．
    for (TpgNode* node = f->node(); node != NULL; node = node->imm_dom()) {
      Literal dlit(node->dvar(), false);
      engine.assumption_add(dlit);
      //assump_base.push_back(dlit);
    }

    cout << " detecting for " << f->str() << endl;
    solve(engine, f);
    cout << "   done" << endl;

#if 0
    // 必要割当を求める．
    TestVector* tv = last_pat();
    ymuint ni = tv->input_num();
    vector<Bool3> tmp_model;
    for (ymuint i = 0; i < ni; ++ i) {
      if ( tv->val3(i) == kValX ) {
	continue;
      }
      engine.assumption_begin(assump_base.size() + 1);
      for (vector<Literal>::iterator p = assump_base.begin();
	   p != assump_base.end(); ++ p) {
	engine.assumption_add(*p);
      }
      bool inv = (tv->val3(i) == kVal1);
      Literal alit(network.input(i)->gvar(), inv);
      engine.assumption_add(alit);

      Bool3 tmp_stat = engine.solve(tmp_model);
    }
    cout << "   detecting mandatory assignment done" << endl;
#endif
  }
  clear_node_mark();

  for (vector<TpgNode*>::const_iterator p = mFaultNodeList.begin();
       p != mFaultNodeList.end(); ++ p) {
    TpgNode* node = *p;
    node->clear_oifvar();
  }
}

END_NAMESPACE_YM_SATPG