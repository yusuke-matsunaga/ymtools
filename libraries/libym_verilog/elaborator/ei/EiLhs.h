#ifndef LIBYM_VERILOG_ELABORATOR_EI_EILHS_H
#define LIBYM_VERILOG_ELABORATOR_EI_EILHS_H

/// @file libym_verilog/elaborator/ei/EiLhs.h
/// @brief EiLhs のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: EiLhs.h 2507 2009-10-17 16:24:02Z matsunaga $
///
/// Copyright (C) 2005-2008 Yusuke Matsunaga
/// All rights reserved.


#include "ElbLhs.h"


BEGIN_NAMESPACE_YM_VERILOG

//////////////////////////////////////////////////////////////////////
/// @class EiLhs EiLhs.h "EiLhs.h"
/// @brief 連結演算子の左辺式を表すクラス
//////////////////////////////////////////////////////////////////////
class EiLhs :
  public EiExpr
{
  friend class EiFactory;
private:

  /// @brief コンストラクタ
  /// @param[in] expr 対応する式
  /// @param[in] elem_num 要素数
  /// @param[in] elem_array 要素の配列
  EiLhsConcat(ElbExpr* expr,
	      ymuint elem_num,
	      ElbExpr** elem_array);

  /// @brief デストラクタ
  virtual
  ~EiLhsConcat();


public:
  //////////////////////////////////////////////////////////////////////
  // ElbLhs の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 左辺式の要素数の取得
  /// @note 通常は1だが，連結演算子の場合はその子供の数となる．
  /// @note ただし，連結演算の入れ子はすべて平坦化して考える．
  virtual
  ymuint
  elem_num() const;

  /// @brief 左辺式の要素の取得
  /// @param[in] pos 位置 ( 0 <= pos < lhs_elem_num() )
  /// @note 連結演算子の見かけと異なり LSB 側が0番めの要素となる．
  virtual
  ElbExpr*
  elem(ymuint pos) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 要素数
  ymuint mNum;

  // 要素の配列
  ElbExpr** mArray;

};

END_NAMESPACE_YM_VERILOG

#endif // LIBYM_VERILOG_ELABORATOR_EI_EILHS_H
