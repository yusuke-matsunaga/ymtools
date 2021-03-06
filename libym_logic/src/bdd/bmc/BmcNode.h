﻿#ifndef BMCNODE_H
#define BMCNODE_H

/// @file BmcNode.h
/// @brief BmcNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmLogic/Bdd.h"


BEGIN_NAMESPACE_YM_BDD

//////////////////////////////////////////////////////////////////////
// BDDのノードを表す構造体
//////////////////////////////////////////////////////////////////////
class BmcNode
{
  friend class BddMgrClassic;

public:
  //////////////////////////////////////////////////////////////////////
  // 定数の定義
  // mRefMarkにパックされる情報
  // 最上位ビットから: positive-mark
  //                   negative-mark
  //                   リンクオーバーフロービット
  //                   残りビットは全てリンク数を表す．
  //////////////////////////////////////////////////////////////////////
  static
  const int kPBit = sizeof(ymuint32) * 8 - 1;
  static
  const int kNBit = sizeof(ymuint32) * 8 - 2;
  static
  const int kLMaxBit = sizeof(ymuint32) * 8 - 3;
  static
  const ymuint32 kPMask = 1UL << kPBit;
  static
  const ymuint32 kNMask = 1UL << kNBit;
  static
  const ymuint32 kLMax = 1UL << kLMaxBit;
  static
  const ymuint32 kLMax_1 = kLMax - 1;
  static
  const ymuint32 kLMask = kLMax | kLMax_1;

public:

  // 中間ノードの場合に0枝/1枝を得る．
  BddEdge
  edge0() const;

  BddEdge
  edge1() const;

  BddEdge
  edge0(tPol p) const;

  BddEdge
  edge1(tPol p) const;

  // レベルを得る．
  ymuint
  level() const;

  // p-mark が付いていたらtrueを返す
  bool
  pmark() const;

  // n-mark が付いていたらtrueを返す
  bool
  nmark() const;

  // p-mark を付ける
  // val が非0でセット,0でリセット
  void
  pmark(int val);

  // n-mark を付ける
  // val が非0でセット,0でリセット
  void
  nmark(int val);

  // 両方のマークを消す
  void
  rst_mark();

  // リンク数を得る．
  ymuint
  refcount() const;

  // 参照されていない時にtrueを返す
  bool
  noref() const;


private:

  // リンク数が最大値を越えている時に0，それ以外には1を返す．
  int
  linkdelta() const;

  // リンク数を増やす(オーバーフロー時は何もしない)
  // 結果のリンク数を返す．
  ymuint
  linkinc();

  // リンク数を減らす(オーバーフロー時は何もしない)
  // 結果のリンク数を返す．
  ymuint
  linkdec();

  // コンストラクタ
  // BddMgrClassic以外はこのオブジェクトを生成できない．
  BmcNode();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 0枝
  BddEdge mEdge0;

  // 1枝
  BddEdge mEdge1;

  // レベル
  ymuint32 mLevel;

  // 参照回数＋α(上の定数を参照)
  ymuint32 mRefMark;

  // 節点テーブル用のリンクポインタ
  BmcNode* mLink;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// 中間ノードの場合に左右の子供への枝を得る．
inline
BddEdge
BmcNode::edge0() const
{
  return mEdge0;
}
inline
BddEdge
BmcNode::edge1() const
{
  return mEdge1;
}
inline
BddEdge
BmcNode::edge0(tPol p) const
{
  return BddEdge(mEdge0, p);
}
inline
BddEdge
BmcNode::edge1(tPol p) const
{
  return BddEdge(mEdge1, p);
}

// レベルを得る
inline
ymuint
BmcNode::level() const
{
  return mLevel;
}

// p-mark が付いていたらtrueを返す
inline
bool
BmcNode::pmark() const
{
  return ((mRefMark >> kPBit) & 1UL);
}

// n-mark が付いていたらtrueを返す
inline
bool
BmcNode::nmark() const
{
  return ((mRefMark >> kNBit) & 1UL);
}

// p-mark を付ける
inline
void
BmcNode::pmark(int val)
{
  if ( val ) {
    mRefMark |= kPMask;
  }
  else {
    mRefMark &= ~kPMask;
  }
}

// n-mark を付ける
inline
void
BmcNode::nmark(int val)
{
  if ( val ) {
    mRefMark |= kNMask;
  }
  else {
    mRefMark &= ~kNMask;
  }
}

// 両方のマークを消す
inline
void
BmcNode::rst_mark()
{
  mRefMark &= ~(kPMask | kNMask);
}

// リンク数を得る．
inline
ymuint
BmcNode::refcount() const
{
  return static_cast<ymuint>(mRefMark & kLMask);
}

// 参照されていない時にtrueを返す
inline
bool
BmcNode::noref() const
{
  return (mRefMark & kLMax_1) == 0UL;
}

// リンク数が最大値を越えている時に0，それ以外には1を返す．
inline
int
BmcNode::linkdelta() const
{
  return static_cast<int>(((~mRefMark) >> kLMaxBit) & 1);
}

// リンク数を増やす(オーバーフロー時は何もしない)
inline
ymuint
BmcNode::linkinc()
{
  int d = linkdelta();
  return (mRefMark += d) & kLMask;
}

// リンク数を減らす(オーバーフロー時は何もしない)
inline
ymuint
BmcNode::linkdec()
{
  int d = linkdelta();
  return (mRefMark -= d) & kLMask;
}

// コンストラクタ
// BddMgrClassic以外はこのオブジェクトを生成できない．
inline
BmcNode::BmcNode()
{
}

END_NAMESPACE_YM_BDD

BEGIN_NAMESPACE_HASH
// BmcNode へのポインタをキーにしたハッシュ関数クラスの定義
template <>
struct hash<nsYm::nsBdd::BmcNode*>
{
  ymuint
  operator()(nsYm::nsBdd::BmcNode* node) const
  {
    return reinterpret_cast<ympuint>(node)/sizeof(void*);
  }
};
END_NAMESPACE_HASH

#endif // BMCNODE_H
