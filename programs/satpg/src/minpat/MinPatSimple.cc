﻿
/// @file MinPatSimple.cc
/// @brief MinPatSimple の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2013-2014, 2015 Yusuke Matsunaga
/// All rights reserved.


#include "MinPatSimple.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "FaultMgr.h"
#include "FaultAnalyzer.h"
#include "FgMgr.h"
#include "TpgCnf1.h"
#include "YmUtils/RandGen.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief インスタンスを生成する関数
MinPat*
new_MinPat2()
{
  return new MinPatSimple();
}

BEGIN_NONAMESPACE

struct FaultLt
{
  bool
  operator()(const pair<double, TpgFault*>& left,
	     const pair<double, TpgFault*>& right)
  {
    return left.first > right.first;
  }
};

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス MinPatSimple
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MinPatSimple::MinPatSimple()
{
}

// @brief デストラクタ
MinPatSimple::~MinPatSimple()
{
}

// @brief 初期化を行う．
// @param[in] network 対象のネットワーク
// @param[in] tvmgr テストベクタマネージャ
// @param[in] fmgr 故障マネージャ
// @param[in] fsim2 2値の故障シミュレータ(検証用)
// @param[in] tv_list テストベクタのリスト
// @return 支配故障数を返す．
ymuint
MinPatSimple::init(TpgNetwork& network,
		   TvMgr& tvmgr,
		   FaultMgr& fmgr,
		   Fsim& fsim2,
		   vector<TestVector*>& tv_list)
{
  mMaxNodeId = network.max_node_id();

  FaultAnalyzer analyzer;

  analyzer.set_verbose(verbose());

#if 0
  const vector<TpgFault*>& fault_list = fmgr.det_list();
#else
  vector<TpgFault*> f_list2;
  for (ymuint i = 0; i < network.active_node_num(); ++ i) {
    const TpgNode* node = network.active_node(i);
    ymuint ni = node->fanin_num();
    bool has_ncfault = false;
    for (ymuint j = 0; j < ni; ++ j) {
      TpgFault* f0 = node->input_fault(0, j);
      if ( f0 != NULL ) {
	if ( f0->is_rep() && f0->status() == kFsDetected ) {
	  f_list2.push_back(f0);
	}
	if ( node->nval() == kVal0 && f0->rep_fault()->status() == kFsDetected ) {
	  has_ncfault = true;
	}
      }
      TpgFault* f1 = node->input_fault(1, j);
      if ( f1 != NULL ) {
	if ( f1->is_rep() && f1->status() == kFsDetected ) {
	  f_list2.push_back(f1);
	}
	if ( node->nval() == kVal1 && f1->rep_fault()->status() == kFsDetected ) {
	  has_ncfault = true;
	}
      }
    }
    TpgFault* f0 = node->output_fault(0);
    if ( f0 != NULL && f0->is_rep() && f0->status() == kFsDetected ) {
      if ( node->noval() != kVal0 || !has_ncfault ) {
	f_list2.push_back(f0);
      }
    }
    TpgFault* f1 = node->output_fault(1);
    if ( f1 != NULL && f1->is_rep() && f1->status() == kFsDetected ) {
      if ( node->noval() != kVal1 || !has_ncfault ) {
	f_list2.push_back(f1);
      }
    }
  }
  const vector<TpgFault*>& fault_list = f_list2;
#endif

  analyzer.init(mMaxNodeId, fault_list);

  ymuint npat0 = 10000;
  RandGen rg;
  analyzer.get_pat_list(fsim2, tvmgr, tv_list, rg, npat0);

  bool dom_fast = true;
  analyzer.get_dom_faults(dom_fast);

  analyzer.analyze_faults();

  ymuint sample_num = 2000;
  vector<double> conf_prob_array;
  analyzer.estimate_conflict(sample_num, conf_prob_array);

  const vector<TpgFault*>& src_list = analyzer.dom_fault_list();
  ymuint nf = src_list.size();
  vector<pair<double, TpgFault*> > tmp_list(nf);
  for (ymuint i = 0; i < nf; ++ i) {
    TpgFault* f = src_list[i];
    double cnum = conf_prob_array[f->id()];
    tmp_list[i] = make_pair(cnum, f);
  }
  sort(tmp_list.begin(), tmp_list.end(), FaultLt());
  mFaultList.clear();
  mFaultList.resize(nf);
  for (ymuint i = 0; i < nf; ++ i) {
    mFaultList[i] = tmp_list[i].second;
  }

  return nf;
}

// @brief 最初の故障を選ぶ．
TpgFault*
MinPatSimple::get_first_fault()
{
  mNextPos = 1;
  return mFaultList[0];
}

// @brief 次に処理すべき故障を選ぶ．
// @param[in] fgmgr 故障グループを管理するオブジェクト
//
// 故障が残っていなければ NULL を返す．
TpgFault*
MinPatSimple::get_next_fault(FgMgr& fgmgr)
{
  if ( mNextPos < mFaultList.size() ) {
    TpgFault* fault = mFaultList[mNextPos];
    ++ mNextPos;
    return fault;
  }
  else {
    return NULL;
  }
}

// @brief 故障を追加するグループを選ぶ．
// @param[in] fgmgr 故障グループを管理するオブジェクト
// @param[in] fault 故障
//
// グループが見つからなければ fgmgr.group_num() を返す．
ymuint
MinPatSimple::find_group(FgMgr& fgmgr,
			 TpgFault* fault)
{
  TpgCnf1 tpg_cnf(string(), string(), NULL);
  tpg_cnf.make_fval_cnf(fault, mMaxNodeId);

  ymuint ng = fgmgr.group_num();
  for (ymuint gid = 0; gid < ng; ++ gid) {
    const NodeValList& suf_list0 = fgmgr.suf_list(gid);
    if ( tpg_cnf.check_intersect(suf_list0) ) {
      return gid;
    }
  }
  return ng;
}

END_NAMESPACE_YM_SATPG