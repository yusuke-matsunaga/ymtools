﻿
/// @file IdMgr.cc
/// @brief IdMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "IdMgr.h"


BEGIN_NAMESPACE_YM_BDD

//////////////////////////////////////////////////////////////////////
// クラス IdMgr
//////////////////////////////////////////////////////////////////////

// コンストラクタ
IdMgr::IdMgr()
{
  mNext = 0;
}

// デストラクタ
IdMgr::~IdMgr()
{
}

// 登録された節点数を返す．
ymuint64
IdMgr::num() const
{
  return mNext;
}

// e がすでに ID 番号を持っていたら true を返す．
bool
IdMgr::has_id(BddEdge e) const
{
  BddEdge e_p(e);
  e_p.normalize();
  ymuint64 id;
  return mMap.find(e_p, id);
}

// e の ID 番号を返す．
// 未登録ならば新しい番号を割り振る．
ymuint64
IdMgr::id(BddEdge e)
{
  BddEdge e_p(e);
  e_p.normalize();
  ymuint64 id;
  if ( !mMap.find(e_p, id) ) {
    id = mNext;
    ++ mNext;
    mMap.add(e_p, id);
  }
  return id;
}

// クリアする．
void
IdMgr::clear()
{
  mMap.clear();
  mNext = 0;
}

END_NAMESPACE_YM_BDD
