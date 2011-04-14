#ifndef LIBYM_VERILOG_ELB_ELBLHS_H
#define LIBYM_VERILOG_ELB_ELBLHS_H

/// @file libym_verilog/elaborator/include/ElbLhs.h
/// @brief ElbLhs のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: ElbLhs.h 2507 2009-10-17 16:24:02Z matsunaga $
///
/// Copyright (C) 2005-2010 Yusuke Matsunaga
/// All rights reserved.


#include "ElbFwd.h"


BEGIN_NAMESPACE_YM_VERILOG

//////////////////////////////////////////////////////////////////////
/// @class ElbLhs ElbLhs.h "ElbLhs.h"
/// @brief 左辺を表すクラス
//////////////////////////////////////////////////////////////////////
class ElbLhs
{
protected:

  /// @brief コンストラクタ
  /// @param[in] expr 対応する式
  ElbLhs(ElbExpr* expr);

  /// @brief デストラクタ
  virtual
  ~ElbLhs() { }


public:
  //////////////////////////////////////////////////////////////////////
  // ElbLhs の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 式を返す．
  ElbExpr*
  _expr() const;

  /// @brief 左辺式の要素数の取得
  /// @note 通常は1だが，連結演算子の場合はその子供の数となる．
  /// @note ただし，連結演算の入れ子はすべて平坦化して考える．
  virtual
  ymuint
  elem_num() const = 0;

  /// @brief 左辺式の要素の取得
  /// @param[in] pos 位置 ( 0 <= pos < lhs_elem_num() )
  /// @note 連結演算子の見かけと異なり LSB 側が0番めの要素となる．
  virtual
  ElbExpr*
  elem(ymuint pos) const = 0;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対応する式
  ElbExpr* mExpr;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] expr 対応する式
inline
ElbLhs::ElbLhs(ElbExpr* expr) :
  mExpr(expr)
{
}

// @brief 式を返す．
inline
ElbExpr*
ElbLhs::_expr() const
{
  return mExpr;
}

END_NAMESPACE_YM_VERILOG

#endif // LIBYM_VERILOG_ELB_ELBLHS_H