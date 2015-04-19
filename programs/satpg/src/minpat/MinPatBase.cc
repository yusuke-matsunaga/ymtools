﻿
/// @file MinPatBase.cc
/// @brief MinPatBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2013-2014 Yusuke Matsunaga
/// All rights reserved.


#include "MinPatBase.h"
#include "FgMgr.h"
#include "Compactor.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TvMgr.h"
#include "Verifier.h"
#include "GvalCnf.h"
#include "FvalCnf.h"
#include "SatEngine.h"
#include "NodeSet.h"
#include "ModelValMap.h"

#include "YmUtils/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス MinPatBase
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MinPatBase::MinPatBase(bool group_dominance)
{
  mVerbose = 0;
  mGroupDominance = group_dominance;
  mDomMethod = 2;
}

// @brief デストラクタ
MinPatBase::~MinPatBase()
{
}

// @brief テストベクタの最小化を行なう．
// @param[in] network 対象のネットワーク
// @param[in] tvmgr テストベクタマネージャ
// @param[in] fsim2 2値の故障シミュレータ(検証用)
// @param[out] tv_list テストベクタのリスト
// @param[out] stats 実行結果の情報を格納する変数
void
MinPatBase::run(TpgNetwork& network,
		TvMgr& tvmgr,
		Fsim& fsim2,
		vector<TestVector*>& tv_list,
		USTime& time)
{
  StopWatch total_timer;
  total_timer.start();

  mMaxNodeId = network.max_node_id();

  mAnalyzer.set_verbose(verbose());
  mAnalyzer.init(network, tvmgr);

  vector<TpgFault*> fault_list;
  init(network, tvmgr, fsim2, fault_list);

  StopWatch local_timer;
  local_timer.start();

  FgMgr fgmgr(mMaxNodeId);
  vector<ymuint> group_list;
  ymuint nf = fault_num();

  { // 最初の故障を選ぶ
    TpgFault* fault = get_first_fault();

    // 最初のグループを作る．
    ymuint gid = fgmgr.new_group();
    group_list.push_back(gid);

    // 故障を追加する．
    fgmgr.add_fault(gid, fault);
  }

  // 未処理の故障がある限り以下の処理を繰り返す．
  for (ymuint c = 0; ; ++ c) {

    if ( verbose() > 1 ) {
      cout << "\r   " << setw(6) << c << " / " << setw(6) << nf
	   << " : " << setw(6) << fgmgr.group_num();
      cout.flush();
    }

    // 故障を選ぶ．
    TpgFault* fault = get_next_fault(fgmgr, group_list);
    if ( fault == NULL ) {
      break;
    }

    // 故障を追加できるグループを見つける．
    ymuint gid = find_group(fgmgr, fault, group_list);
    if ( gid == fgmgr.group_num() ) {
      // 見つからなかった．
      // 新たなグループを作る．
      gid = fgmgr.new_group();
      group_list.push_back(gid);
    }

    // 故障を追加する．
    fgmgr.add_fault(gid, fault);
  }

  local_timer.stop();
  if ( verbose() > 0 ) {
    if ( verbose() > 1 ) {
      cout << endl;
    }
    cout << " # of fault groups = " << group_list.size() << endl;
    cout << "CPU time (coloring)              " << local_timer.time() << endl;
  }

  // 後処理
  local_timer.reset();
  local_timer.start();

  Compactor compactor;

  vector<ymuint> new_group_list;
  compactor.run(fgmgr, mMaxNodeId, group_list, new_group_list);

  local_timer.stop();
  if ( verbose() > 0 ) {
    cout << " # of fault groups = " << new_group_list.size() << endl;
    cout << "CPU time (compaction)              " << local_timer.time() << endl;
  }

  // テストパタンを作る．
  local_timer.reset();
  local_timer.start();

  ymuint new_ng = new_group_list.size();
  tv_list.clear();
  tv_list.reserve(new_ng);
  for (ymuint i = 0; i < new_ng; ++ i) {
    ymuint gid = new_group_list[i];
    const NodeValList& suf_list = fgmgr.sufficient_assignment(gid);
    TestVector* tv = tvmgr.new_vector();
    make_testvector(network, suf_list, tv);
    tv_list.push_back(tv);
  }

  local_timer.stop();
  if ( verbose() > 0 ) {
    cout << "CPU time (testvector generation) " << local_timer.time() << endl;
  }

  {
    // 検証しておく．
    Verifier ver;
    if ( ver.check(fsim2, fault_list, tv_list) ) {
      cout << "No Errors" << endl;
    }
  }

  total_timer.stop();
  time = total_timer.time();
}

// @brief verbose フラグをセットする．
void
MinPatBase::set_verbose(int verbose)
{
  mVerbose = verbose;
}

// @brief verbose フラグを得る．
int
MinPatBase::verbose() const
{
  return mVerbose;
}

// @brief dom_method を指定する．
void
MinPatBase::set_dom_method(ymuint dom_method)
{
  mDomMethod = dom_method;
}

// @brief get_dom_faults() のアルゴリズムを指定する．
ymuint
MinPatBase::dom_method() const
{
  return mDomMethod;
}

// @brief 故障を追加するグループを選ぶ．
// @param[in] fgmgr 故障グループを管理するオブジェクト
// @param[in] fault 故障
// @param[in] group_list 現在のグループリスト
//
// グループが見つからなければ fgmgr.group_num() を返す．
ymuint
MinPatBase::find_group(FgMgr& fgmgr,
		       TpgFault* fault,
		       const vector<ymuint>& group_list)
{
  if ( mGroupDominance ) {
    GvalCnf gval_cnf(mMaxNodeId);
    FvalCnf fval_cnf(mMaxNodeId, gval_cnf);
    SatEngine engine(string(), string(), NULL);

    // fault が見つからない条件を作る．
    fval_cnf.make_cnf(engine, fault, kVal0);

    for (ymuint i = 0; i < group_list.size(); ++ i) {
      ymuint gid = group_list[i];
      const NodeValList& suf_list0 = fgmgr.sufficient_assignment(gid);
      if ( engine.check_sat(gval_cnf, suf_list0) == kB3False ) {
	// suf_lib0 のもとでは必ず見つかるということ．
	return gid;
      }
    }
  }

  GvalCnf gval_cnf(mMaxNodeId);
  FvalCnf fval_cnf(mMaxNodeId, gval_cnf);
  SatEngine engine(string(), string(), NULL);

  // fault が見つかる条件を作る．
  fval_cnf.make_cnf(engine, fault, kVal1);

  for (ymuint i = 0; i < group_list.size(); ++ i) {
    ymuint gid = group_list[i];
    const NodeValList& suf_list0 = fgmgr.sufficient_assignment(gid);
    if ( engine.check_sat(gval_cnf, suf_list0) == kB3True ) {
      return gid;
    }
  }
  return fgmgr.group_num();
}

// @brief テストパタンを作る．
// @param[in] gid グループ番号
// @param[in] network ネットワーク
// @param[in] tv テストベクタ
void
MinPatBase::make_testvector(TpgNetwork& network,
			    const NodeValList& suf_list,
			    TestVector* tv)
{
  GvalCnf gval_cnf(mMaxNodeId);

  SatEngine engine(string(), string(), NULL);

  engine.assumption_begin();
  gval_cnf.add_assumption(engine, suf_list);

  vector<Bool3> sat_model;
  SatStats sat_stats;
  USTime sat_time;
  Bool3 sat_ans = engine.solve(sat_model, sat_stats, sat_time);
  ASSERT_COND ( sat_ans == kB3True );

  const VidMap& var_map = gval_cnf.var_map();
  ModelValMap val_map(var_map, var_map, sat_model);
  ymuint ni = network.input_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* node = network.input(i);
    ymuint input_id = node->input_id();
    Val3 val;
    if ( var_map(node) == kVarIdIllegal ) {
      val = kVal0;
    }
    else {
      val = val_map.gval(node);
    }
    tv->set_val(input_id, val);
  }
}

// @brief 故障解析器を返す．
FaultAnalyzer&
MinPatBase::analyzer()
{
  return mAnalyzer;
}

END_NAMESPACE_YM_SATPG
