#ifndef GVALCNF_H
#define GVALCNF_H

/// @file GvalCnf.h
/// @brief GvalCnf のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "satpg_nsdef.h"
#include "GenVidMap.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class GvalCnf GvalCnf.h "GvalCnf.h"
/// @brief 正常回路のCNF式を作るためのクラス
//////////////////////////////////////////////////////////////////////
class GvalCnf
{
  friend class SatEngine;
  friend class FvalCnf;

public:

  /// @brief コンストラクタ
  /// @param[in] max_node_id ノード番号の最大値
  GvalCnf(ymuint max_node_id);

  /// @brief デストラクタ
  ~GvalCnf();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  /// @param[in] max_node_id ノード番号の最大値
  void
  init(ymuint max_node_id);

  /// @brief ノード番号の最大値を返す．
  ymuint
  max_node_id() const;

  /// @brief 変数マップを得る．
  const VidMap&
  var_map() const;

  /// @brief 変数番号を得る．
  /// @param[in] node ノード
  VarId
  var(const TpgNode* node) const;

  /// @brief ノードのマークを調べる．
  /// @param[in] node ノード
  bool
  mark(const TpgNode* node) const;

  /// @brief ノードにマークをつける．
  /// @param[in] node ノード
  void
  set_mark(const TpgNode* node);

  /// @brief ノードに正常値用の変数番号を割り当てる．
  /// @param[in] node ノード
  /// @param[in] var 変数番号
  void
  set_vid(const TpgNode* node,
	  VarId var);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////
#if 0
  /// @brief NodeSet の内容に応じてCNFを作る．
  /// @param[in] engine SATエンジン
  /// @param[in] node_set 対象のノード集合
  ///
  /// 具体的には tfi_tfo_node() を対象にする．
  void
  make_cnf(SatEngine& engine,
	   const NodeSet& node_set);

  /// @brief ノードのTFI全体のCNFを作る．
  /// @param[in] engine SATエンジン
  /// @param[in] node ノード
  void
  make_cnf(SatEngine& engine,
	   const TpgNode* node);

  /// @brief 割当リストに対応する仮定を追加する．
  /// @param[in] engine SATエンジン
  /// @param[in] assign_list 割当リスト
  void
  add_assumption(SatEngine& engine,
		 const NodeValList& assign_list);
#endif

private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード番号の最大値
  ymuint mMaxId;

  // 処理済みのノードの印
  vector<bool> mMark;

  // 変数マップ
  GenVidMap mVarMap;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ノード番号の最大値を返す．
ymuint
GvalCnf::max_node_id() const
{
  return mMaxId;
}

// @brief 変数マップを得る．
const VidMap&
GvalCnf::var_map() const
{
  return mVarMap;
}

// @brief 変数番号を得る．
// @param[in] node ノード
inline
VarId
GvalCnf::var(const TpgNode* node) const
{
  return mVarMap(node);
}

// @brief ノードのマークを調べる．
// @param[in] node ノード
inline
bool
GvalCnf::mark(const TpgNode* node) const
{
  return mMark[node->id()];
}

// @brief ノードにマークをつける．
// @param[in] node ノード
inline
void
GvalCnf::set_mark(const TpgNode* node)
{
  mMark[node->id()] = true;
}

// @brief ノードに正常値用の変数番号を割り当てる．
// @param[in] node ノード
// @param[in] var 変数番号
inline
void
GvalCnf::set_vid(TpgNode* node,
		 VarId var)
{
  ASSERT_COND( id < mMaxId );
  mVarMap.set_vid(node, var);
}

END_NAMESPACE_YM_SATPG

#endif // GVALCNF_H
