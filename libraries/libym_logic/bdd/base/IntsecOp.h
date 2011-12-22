#ifndef INTSECOP_H
#define INTSECOP_H

/// @file IntsecOp.h
/// @brief IntsecOp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "BddOp.h"


BEGIN_NAMESPACE_YM_BDD

//////////////////////////////////////////////////////////////////////
/// @class IntsecOp IntsecOp.h "IntsecOp.h"
/// @brief AND 演算行うクラス
//////////////////////////////////////////////////////////////////////
class IntsecOp :
  public BddBinOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] mgr マネージャ
  IntsecOp(BddMgrImpl* mgr);

  /// @brief デストラクタ
  virtual
  ~IntsecOp();


public:
  //////////////////////////////////////////////////////////////////////
  // メインの関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 演算を行う関数
  /// @param[in] left, right オペランド
  /// @return 演算結果を買えす．
  virtual
  BddEdge
  apply(BddEdge left,
	BddEdge right);


private:
  //////////////////////////////////////////////////////////////////////
  // 下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 実際の演算を行う関数
  BddEdge
  apply_step(BddEdge f,
	     BddEdge g);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // マネージャ
  BddMgrImpl* mMgr;

};

END_NAMESPACE_YM_BDD

#endif // INTSECOP_H