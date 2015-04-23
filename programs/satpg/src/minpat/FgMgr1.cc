
/// @file FgMgr1.cc
/// @brief FgMgr1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "FgMgr1.h"

#include "TpgFault.h"
#include "NodeSet.h"
#include "GvalCnf.h"
#include "FvalCnf.h"
#include "SatEngine.h"
#include "YmUtils/HashSet.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス FgMgr1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_node_id ノード番号の最大値 + 1
FgMgr1::FgMgr1(ymuint max_node_id) :
  mMaxNodeId(max_node_id)
{
}

// @brief デストラクタ
FgMgr1::~FgMgr1()
{
  clear();
}

// @brief クリアする．
void
FgMgr1::clear()
{
  for (ymuint gid = 0; gid < mGroupList.size(); ++ gid) {
    delete mGroupList[gid];
  }
  mGroupList.clear();
}

// @brief 現在のグループ数を返す．
ymuint
FgMgr1::group_num() const
{
  return mGroupList.size();
}

// @brief 新しいグループを作る．
// @return グループ番号を返す．
ymuint
FgMgr1::new_group()
{
  FaultGroup* fg = new FaultGroup;
  bool found = false;
  for (ymuint i = 0; i < mGroupList.size(); ++ i) {
    if ( mGroupList[i] == NULL ) {
      fg->mId = i;
      mGroupList[i] = fg;
      found = true;
      break;
    }
  }
  if ( !found ) {
    fg->mId = mGroupList.size();
    mGroupList.push_back(fg);
  }
  return fg->mId;
}

// @brief グループを複製する．
// @param[in] src_gid 複製元のグループ番号
// @return 新しいグループ番号を返す．
ymuint
FgMgr1::duplicate_group(ymuint src_gid)
{
  ASSERT_COND( src_gid < mGroupList.size() );
  FaultGroup* src_fg = mGroupList[src_gid];
  ASSERT_COND( src_fg != NULL );
  ymuint gid = new_group();
  FaultGroup* dst_fg = mGroupList[gid];
  dst_fg->mFaultList = src_fg->mFaultList;
  dst_fg->mFaultSufList = src_fg->mFaultSufList;
  dst_fg->mSufList = src_fg->mSufList;
  return gid;
}

// @brief グループを置き換える．
// @param[in] old_gid 置き換え対象のグループ番号
// @param[in] new_gid 置き換えるグループ番号
//
// new_gid は削除される．
void
FgMgr1::replace_group(ymuint old_gid,
		     ymuint new_gid)
{
  delete_group(old_gid);

  ASSERT_COND( new_gid < mGroupList.size() );
  FaultGroup* new_fg = mGroupList[new_gid];
  ASSERT_COND( new_fg != NULL );
  new_fg->mId = old_gid;
  mGroupList[old_gid] = new_fg;
  mGroupList[new_gid] = NULL;
}

// @brief グループを削除する．
// @param[in] gid グループ番号
void
FgMgr1::delete_group(ymuint gid)
{
  ASSERT_COND( gid < mGroupList.size() );
  FaultGroup* fg = mGroupList[gid];
  ASSERT_COND( fg != NULL );
  delete fg;
  mGroupList[gid] = NULL;
}

// @brief 新たな条件なしで追加できる既存グループを見つける．
// @param[in] fault 対象の故障
// @param[in] group_list 探索最小のグループ番号のリスト
// @param[in] first_hit 最初のグループのみを求めるとき true にするフラグ
// @param[out] gid_list 対象のグループ番号を収めるリスト
// @return 最初のグループ番号を返す．
//
// 見つからない場合は group_num() を返す．
// gid_list は first_hit == true の時，意味を持たない．
ymuint
FgMgr1::find_dom_group(const TpgFault* fault,
		      const vector<ymuint>& group_list,
		      bool first_hit,
		      vector<ymuint>& gid_list)
{
  GvalCnf gval_cnf(mMaxNodeId);
  FvalCnf fval_cnf(mMaxNodeId, gval_cnf);
  SatEngine engine(string(), string(), NULL);

  NodeSet node_set;
  node_set.mark_region(mMaxNodeId, fault->node());

  // fault が見つからない条件を作る．
  engine.make_fval_cnf(fval_cnf, fault, node_set, kVal0);

  ymuint first_gid = group_num();
  for (ymuint i = 0; i < group_list.size(); ++ i) {
    ymuint gid = group_list[i];
    const NodeValList& suf_list0 = sufficient_assignment(gid);
    if ( engine.check_sat(gval_cnf, suf_list0) == kB3False ) {
      // suf_lib0 のもとでは必ず見つかるということ．
      if ( first_gid == group_num() ) {
	first_gid = gid;
	if ( first_hit ) {
	  break;
	}
      }
      gid_list.push_back(gid);
    }
  }
  return first_gid;
}

// @brief 追加できる既存グループを見つける．
// @param[in] fault 対象の故障
// @param[in] group_list 探索最小のグループ番号のリスト
// @param[in] first_hit 最初のグループのみを求めるとき true にするフラグ
// @param[out] gid_list 対象のグループ番号を収めるリスト
// @return 最初のグループ番号を返す．
//
// 見つからない場合は group_num() を返す．
// gid_list は first_hit == true の時，意味を持たない．
ymuint
FgMgr1::find_group(const TpgFault* fault,
		  const vector<ymuint>& group_list,
		  bool first_hit,
		  vector<ymuint>& gid_list)
{
  GvalCnf gval_cnf(mMaxNodeId);
  FvalCnf fval_cnf(mMaxNodeId, gval_cnf);
  SatEngine engine(string(), string(), NULL);

  NodeSet node_set;
  node_set.mark_region(mMaxNodeId, fault->node());

  engine.make_fval_cnf(fval_cnf, fault, node_set, kVal1);

  ymuint first_gid = group_num();
  for (ymuint i = 0; i < group_list.size(); ++ i) {
    ymuint gid = group_list[i];
    const NodeValList& suf_list0 = sufficient_assignment(gid);
    Bool3 sat_ans = engine.check_sat(gval_cnf, suf_list0);
    if ( sat_ans == kB3True ) {
      if ( first_gid == group_num() ) {
	first_gid = gid;
	if ( first_hit ) {
	  break;
	}
      }
      gid_list.push_back(gid);
    }
  }
  return first_gid;
}

// @brief 既存のグループに故障を追加する．
// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
// @param[in] fault 故障
void
FgMgr1::add_fault(ymuint gid,
		 const TpgFault* fault)
{
  ASSERT_COND( gid < group_num() );
  FaultGroup* fg = mGroupList[gid];

  GvalCnf gval_cnf(mMaxNodeId);
  FvalCnf fval_cnf(mMaxNodeId, gval_cnf);
  SatEngine engine(string(), string(), NULL);

  NodeSet node_set;
  node_set.mark_region(mMaxNodeId, fault->node());

  engine.make_fval_cnf(fval_cnf, fault, node_set, kVal1);

  vector<Bool3> sat_model;
  Bool3 sat_ans = engine.check_sat(gval_cnf, fg->mSufList, sat_model);
  ASSERT_COND( sat_ans == kB3True );

  NodeValList suf_list;
  NodeValList pi_suf_list;
  fval_cnf.get_pi_suf_list(sat_model, fault, node_set, suf_list, pi_suf_list);

  fg->add_fault(fault, suf_list, pi_suf_list);
}

// @brief 故障を取り除く
// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
// @param[in] fault_list 故障リスト
void
FgMgr1::delete_fault(ymuint gid,
		    const vector<const TpgFault*>& fault_list)
{
  ASSERT_COND( gid < group_num() );
  FaultGroup* fg = mGroupList[gid];

  HashSet<ymuint> fault_hash;
  for (ymuint i = 0; i < fault_list.size(); ++ i) {
    const TpgFault* fault = fault_list[i];
    fault_hash.add(fault->id());
  }

  fg->mSufList = NodeValList();
  ymuint nf = fg->mFaultList.size();
  ymuint wpos = 0;
  for (ymuint i = 0; i < nf; ++ i) {
    const TpgFault* fault = fg->mFaultList[i];
    if ( fault_hash.check(fault->id()) ) {
      continue;
    }
    if ( wpos != i ) {
      fg->mFaultList[wpos] = fault;
      fg->mFaultSufList[wpos] = fg->mFaultSufList[i];
    }
    ++ wpos;
    fg->mSufList.merge(fg->mFaultSufList[i]);
  }
  fg->mFaultList.erase(fg->mFaultList.begin() + wpos, fg->mFaultList.end());
  fg->mFaultSufList.erase(fg->mFaultSufList.begin() + wpos, fg->mFaultSufList.end());
}

// @brief 故障リストを返す．
// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
const vector<const TpgFault*>&
FgMgr1::fault_list(ymuint gid) const
{
  ASSERT_COND( gid < group_num() );
  FaultGroup* fg = mGroupList[gid];
  return fg->mFaultList;
}

// @brief 十分割当リストを返す．
// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
const NodeValList&
FgMgr1::sufficient_assignment(ymuint gid) const
{
  ASSERT_COND( gid < group_num() );
  FaultGroup* fg = mGroupList[gid];
  return fg->mSufList;
}

// @brief 外部入力上の十分割当リストを返す．
// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
const NodeValList&
FgMgr1::pi_sufficient_assignment(ymuint gid) const
{
  ASSERT_COND( gid < group_num() );
  FaultGroup* fg = mGroupList[gid];
  return fg->mPiSufList;
}

END_NAMESPACE_YM_SATPG