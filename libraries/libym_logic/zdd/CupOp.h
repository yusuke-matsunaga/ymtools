#ifndef CUPOP_H
#define CUPOP_H

/// @file CupOp.h
/// @brief CupOp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ZddOp.h"
#include "ZddMgrImpl.h"
#include "CompTbl.h"


BEGIN_NAMESPACE_YM_ZDD

//////////////////////////////////////////////////////////////////////
/// @class CupOp CupOp.h "CupOp.h"
/// @brief ユニオンを求めるクラス
//////////////////////////////////////////////////////////////////////
class CupOp :
  public ZddBinOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] mgr ZddMgrImpl
  CupOp(ZddMgrImpl& mgr);

  /// @brief デストラクタ
  virtual
  ~CupOp();


public:
  //////////////////////////////////////////////////////////////////////
  // メインの関数
  //////////////////////////////////////////////////////////////////////

  /// @brief \f$\cup\f$演算を行う関数
  /// @param[in] left, right オペランド
  virtual
  ZddEdge
  apply(ZddEdge left,
	ZddEdge right);


private:
  //////////////////////////////////////////////////////////////////////
  // 下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 実際に演算を行う関数
  ZddEdge
  cup_step(ZddEdge f,
	   ZddEdge g);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 親のマネージャ
  ZddMgrImpl& mMgr;

  // 演算テーブル
  CompTbl2 mCupTable;

};

END_NAMESPACE_YM_ZDD

#endif // CUPOP_H