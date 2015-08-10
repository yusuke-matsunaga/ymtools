﻿#ifndef NETWORKS_BDNDFF_H
#define NETWORKS_BDNDFF_H

/// @file YmNetworks/BdnDff.h
/// @brief BdnDff のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmNetworks/bdn.h"


BEGIN_NAMESPACE_YM_NETWORKS_BDN

class BdnAuxData;

//////////////////////////////////////////////////////////////////////
/// @class BdnDff BdnDff.h "YmNetworks/BdnDff.h"
/// @brief D-FFを表すクラス
//////////////////////////////////////////////////////////////////////
class BdnDff :
  public DlElem
{
  friend class BdnMgrImpl;

private:

  /// @brief コンストラクタ
  BdnDff();

  /// @brief デストラクタ
  ~BdnDff();


public:

  /// @brief ID 番号の取得
  /// @return ID 番号を返す．
  /// @note ID 番号は D-FF の生成時に BdnMgr により自動的に割り振られる．
  /// @sa BdnMgr
  ymuint
  id() const;

  /// @brief 名前を返す．
  string
  name() const;

  /// @brief データ出力ノードを返す．
  const BdnNode*
  output() const;

  /// @brief データ出力ノードを返す．
  BdnNode*
  _output();

  /// @brief データ入力ノードを返す．
  const BdnNode*
  input() const;

  /// @brief データ入力ノードを返す．
  BdnNode*
  _input();

  /// @brief クロックのノードを返す．
  const BdnNode*
  clock() const;

  /// @brief クロックのノードを返す．
  BdnNode*
  _clock();

  /// @brief クリア信号のノードを返す．
  /// @note nullptr の場合もある．
  const BdnNode*
  clear() const;

  /// @brief クリア信号のノードを返す．
  /// @note nullptr の場合もある．
  BdnNode*
  _clear();

  /// @brief プリセット信号のノードを返す．
  /// @note nullptr の場合もある．
  const BdnNode*
  preset() const;

  /// @brief プリセット信号のノードを返す．
  /// @note nullptr の場合もある．
  BdnNode*
  _preset();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  ymuint32 mId;

  // 名前
  string mName;

  // データ出力ノード
  BdnNode* mOutput;

  // データ入力ノード
  BdnNode* mInput;

  // クロックノード
  BdnNode* mClock;

  // クリア信号ノード
  BdnNode* mClear;

  // プリセット信号ノード
  BdnNode* mPreset;

  // BdnNode に付加するデータ
  BdnAuxData* mAuxData;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ID 番号の取得
// @return ID 番号を返す．
// @note ID 番号は D-FF の生成時に BdnMgr により自動的に割り振られる．
// @sa BdnMgr
inline
ymuint
BdnDff::id() const
{
  return mId;
}

// @brief 名前を返す．
inline
string
BdnDff::name() const
{
  return mName;
}

// @brief データ出力ノードを返す．
inline
const BdnNode*
BdnDff::output() const
{
  return mOutput;
}

// @brief データ出力ノードを返す．
inline
BdnNode*
BdnDff::_output()
{
  return mOutput;
}

// @brief データ入力ノードを返す．
inline
const BdnNode*
BdnDff::input() const
{
  return mInput;
}

// @brief データ入力ノードを返す．
inline
BdnNode*
BdnDff::_input()
{
  return mInput;
}

// @brief クロックのノードを返す．
inline
const BdnNode*
BdnDff::clock() const
{
  return mClock;
}

// @brief クロックのノードを返す．
inline
BdnNode*
BdnDff::_clock()
{
  return mClock;
}

// @brief クリア信号のノードを返す．
inline
const BdnNode*
BdnDff::clear() const
{
  return mClear;
}

// @brief クリア信号のノードを返す．
inline
BdnNode*
BdnDff::_clear()
{
  return mClear;
}

// @brief プリセット信号のノードを返す．
inline
const BdnNode*
BdnDff::preset() const
{
  return mPreset;
}

// @brief プリセット信号のノードを返す．
inline
BdnNode*
BdnDff::_preset()
{
  return mPreset;
}

END_NAMESPACE_YM_NETWORKS_BDN

#endif // NETWORKS_BDNDFF_H
