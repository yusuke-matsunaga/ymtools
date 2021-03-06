﻿#ifndef DIFFOP2_H
#define DIFFOP2_H

/// @file DiffOp2.h
/// @brief DiffOp2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "BinOp.h"


BEGIN_NAMESPACE_YM_CNFDD

//////////////////////////////////////////////////////////////////////
/// @class DiffOp2 DiffOp2.h "DiffOp2.h"
/// @brief 集合差を求めるクラス
//////////////////////////////////////////////////////////////////////
class DiffOp2 :
  public BinOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] mgr マネージャ
  /// @param[in] dis_op disjuntion operator
  DiffOp2(CNFddMgrImpl& mgr,
	 BinOp* dis_op);

  /// @brief デストラクタ
  virtual
  ~DiffOp2();


public:
  //////////////////////////////////////////////////////////////////////
  // メインの関数
  //////////////////////////////////////////////////////////////////////

  /// @brief \f$\setminus\f$演算を行う関数
  /// @param[in] left, right オペランド
  virtual
  CNFddEdge
  apply(CNFddEdge left,
	CNFddEdge right);


private:
  //////////////////////////////////////////////////////////////////////
  // 下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 実際に演算を行う関数
  CNFddEdge
  apply_step(CNFddEdge f,
	     CNFddEdge g);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // disjunction operator
  BinOp* mDisOp;

};

END_NAMESPACE_YM_CNFDD

#endif // DIFFOP_H
