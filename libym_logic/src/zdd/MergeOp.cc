﻿
/// @file MergeOp.cc
/// @brief MergeOp の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "MergeOp.h"


BEGIN_NAMESPACE_YM_ZDD

//////////////////////////////////////////////////////////////////////
// クラス MergeOp
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] mgr マネージャ
/// @param[in] cup_op union 演算オブジェクト
MergeOp::MergeOp(ZddMgrImpl* mgr,
		 ZddBinOp* cup_op) :
  ZddBinOp(mgr, "merge_table"),
  mCupOp(cup_op)
{
}

// @brief デストラクタ
MergeOp::~MergeOp()
{
}

// @brief マージ演算を行う関数
// @param[in] left, right オペランド
ZddEdge
MergeOp::apply(ZddEdge left,
	       ZddEdge right)
{
  // エラー状態のチェック
  if ( left.is_invalid() ) {
    return left;
  }
  if ( right.is_invalid() ) {
    return right;
  }

  return apply_step(left, right);
}

// cap_op の下請け関数
ZddEdge
MergeOp::apply_step(ZddEdge f,
		    ZddEdge g)
{
  // 特別な場合の処理
  // 1: 片方のZDDが0なら答は0，
  // 2: 片方のZDDが1なら答は他方
  // 3: 同じZDDどうしなら答は自分自身
  if ( f.is_zero() || g.is_zero() ) {
    return ZddEdge::make_zero();
  }
  if ( f.is_one() ) {
    return g;
  }
  if ( g.is_one() || f == g ) {
    return f;
  }

  // この時点で f,g は終端ではない．

  // 演算結果テーブルが当たりやすくなるように順序を正規化する
  if ( f > g ) {
    ZddEdge tmp = f;
    f = g;
    g = tmp;
  }

  ZddEdge ans_e = get(f, g);
  if ( ans_e.is_error() ) {
    // 演算結果テーブルには登録されていない
    ZddEdge f_0, f_1;
    ZddEdge g_0, g_1;
    ymuint level = split(f, g, f_0, f_1, g_0, g_1);
    ZddEdge r_0 = apply_step(f_0, g_0);
    if ( r_0.is_invalid() ) {
      return r_0;
    }

    ZddEdge r_01 = apply_step(f_0, g_1);
    if ( r_01.is_invalid() ) {
      return r_01;
    }
    ZddEdge r_10 = apply_step(f_1, g_0);
    if ( r_10.is_invalid() ) {
      return r_10;
    }
    ZddEdge r_11 = apply_step(f_1, g_1);
    if ( r_11.is_invalid() ) {
      return r_11;
    }
    ZddEdge tmp = mCupOp->apply(r_01, r_10);
    if ( tmp.is_invalid() ) {
      return tmp;
    }
    ZddEdge r_1 = mCupOp->apply(tmp, r_11);
    if ( r_1.is_invalid() ) {
      return r_1;
    }
    ans_e = new_node(level, r_0, r_1);
    put(f, g, ans_e);
  }

  return ans_e;
}

END_NAMESPACE_YM_ZDD
