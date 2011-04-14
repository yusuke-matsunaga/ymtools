#ifndef YM_BDN_BDNDFF_H
#define YM_BDN_BDNDFF_H

/// @file ym_bdn/BdnDff.h
/// @brief BdnDff のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_bdn/bdn_nsdef.h"


BEGIN_NAMESPACE_YM_BDN

class BdnAuxData;

//////////////////////////////////////////////////////////////////////
/// @class BdnDff BdnDff.h "ym_bdn/BdnDff.h"
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
  output();

  /// @brief データ入力ノードを返す．
  const BdnNode*
  input() const;

  /// @brief データ入力ノードを返す．
  BdnNode*
  input();

  /// @brief クロックのノードを返す．
  const BdnNode*
  clock() const;

  /// @brief クロックのノードを返す．
  BdnNode*
  clock();

  /// @brief セット信号のノードを返す．
  /// @note NULL の場合もある．
  const BdnNode*
  set() const;

  /// @brief セット信号のノードを返す．
  /// @note NULL の場合もある．
  BdnNode*
  set();

  /// @brief リセット信号のノードを返す．
  /// @note NULL の場合もある．
  const BdnNode*
  reset() const;

  /// @brief リセット信号のノードを返す．
  /// @note NULL の場合もある．
  BdnNode*
  reset();


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

  // セット信号ノード
  BdnNode* mSet;

  // リセット信号ノード
  BdnNode* mReset;

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
BdnDff::output()
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
BdnDff::input()
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
BdnDff::clock()
{
  return mClock;
}

// @brief セット信号のノードを返す．
// @note NULL の場合もある．
inline
const BdnNode*
BdnDff::set() const
{
  return mSet;
}

// @brief セット信号のノードを返す．
// @note NULL の場合もある．
inline
BdnNode*
BdnDff::set()
{
  return mSet;
}

// @brief リセット信号のノードを返す．
// @note NULL の場合もある．
inline
const BdnNode*
BdnDff::reset() const
{
  return mReset;
}

// @brief リセット信号のノードを返す．
// @note NULL の場合もある．
inline
BdnNode*
BdnDff::reset()
{
  return mReset;
}

END_NAMESPACE_YM_BDN

#endif // YM_BDN_BDNDFF_H