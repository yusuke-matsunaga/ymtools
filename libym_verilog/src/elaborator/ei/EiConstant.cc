﻿
/// @file EiConstant.cc
/// @brief EiExpr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "EiFactory.h"
#include "EiConstant.h"

#include "YmVerilog/VlValue.h"
#include "YmVerilog/pt/PtExpr.h"


BEGIN_NAMESPACE_YM_VERILOG

//////////////////////////////////////////////////////////////////////
// EiFactory の生成関数
//////////////////////////////////////////////////////////////////////

// @brief 定数式を生成する．
// @param[in] pt_expr パース木の定義要素
ElbExpr*
EiFactory::new_Constant(const PtExpr* pt_expr)
{
  tVpiConstType const_type = pt_expr->const_type();
  ymuint size = pt_expr->const_size();
  bool is_signed = false;
  ymuint base = 0;
  switch ( const_type ) {
  case kVpiIntConst:
    if ( pt_expr->const_str() == nullptr ) {
      void* p = mAlloc.get_memory(sizeof(EiIntConst));
      return new (p) EiIntConst(pt_expr, pt_expr->const_uint());
    }
    break;

  case kVpiSignedBinaryConst:
    is_signed = true;
  case kVpiBinaryConst:
    base = 2;
    break;

  case kVpiSignedOctConst:
    is_signed = true;
  case kVpiOctConst:
    base = 8;
    break;

  case kVpiSignedDecConst:
    is_signed = true;
  case kVpiDecConst:
    base = 10;
    break;

  case kVpiSignedHexConst:
    is_signed = true;
  case kVpiHexConst:
    base = 16;
    break;

  case kVpiRealConst:
    {
      void* p = mAlloc.get_memory(sizeof(EiRealConst));
      return new (p) EiRealConst(pt_expr, pt_expr->const_real());
    }

  case kVpiStringConst:
    {
      void* p = mAlloc.get_memory(sizeof(EiStringConst));
      return new (p) EiStringConst(pt_expr, pt_expr->const_str());
    }

  default:
    ASSERT_NOT_REACHED;
    break;
  }

  // ここに来たということはビットベクタ型
  void* p = mAlloc.get_memory(sizeof(EiBitVectorConst));
  return new (p) EiBitVectorConst(pt_expr, const_type,
				  BitVector(size, is_signed, base,
					    pt_expr->const_str()));
}

// @brief genvar 起因の定数式を生成する．
// @param[in] pt_item パース木の定義要素
// @param[in] val 値
ElbExpr*
EiFactory::new_GenvarConstant(const PtExpr* pt_primary,
			      int val)
{
  void* p = mAlloc.get_memory(sizeof(EiIntConst));
  return new (p) EiIntConst(pt_primary, val);
}


//////////////////////////////////////////////////////////////////////
// クラス EiConstant
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] pt_expr パース木の定義要素
EiConstant::EiConstant(const PtExpr* pt_expr) :
  EiExprBase(pt_expr)
{
}

// @brief デストラクタ
EiConstant::~EiConstant()
{
}

// @brief 型の取得
tVpiObjType
EiConstant::type() const
{
  return kVpiConstant;
}

// @brief 定数の時 true を返す．
/// @note このクラスは常に true を返す．
bool
EiConstant::is_const() const
{
  return true;
}

// @brief 要求される式の型を計算してセットする．
// @param[in] type 要求される式の型
// @note 必要であればオペランドに対して再帰的に処理を行なう．
void
EiConstant::_set_reqsize(const VlValueType& type)
{
  // なにもしない．
}

// @brief オペランドを返す．
// @param[in] pos 位置番号
// @note 演算子の時，意味を持つ．
// @note このクラスでは nullptr を返す．
ElbExpr*
EiConstant::_operand(ymuint pos) const
{
  return nullptr;
}


//////////////////////////////////////////////////////////////////////
// クラス EiIntConst
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] pt_expr パース木の定義要素
// @param[in] value 値
EiIntConst::EiIntConst(const PtExpr* pt_expr,
		       int value) :
  EiConstant(pt_expr),
  mValue(value)
{
}

// @brief デストラクタ
EiIntConst::~EiIntConst()
{
}

// @brief 式のタイプを返す．
VlValueType
EiIntConst::value_type() const
{
  return VlValueType::int_type();
}

// @brief 定数の型を返す．
// @note 定数の時，意味を持つ．
tVpiConstType
EiIntConst::constant_type() const
{
  return kVpiIntConst;
}

// @brief 定数値を返す．
// @note kVpiConstant の時，意味を持つ．
// @note それ以外では動作は不定
VlValue
EiIntConst::constant_value() const
{
  return VlValue(mValue);
}


//////////////////////////////////////////////////////////////////////
// クラス EiBitVectorConst
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] pt_expr パース木の定義要素
// @param[in] const_type 定数型
// @param[in] value 値
EiBitVectorConst::EiBitVectorConst(const PtExpr* pt_expr,
				   tVpiConstType const_type,
				   const BitVector& value) :
  EiConstant(pt_expr),
  mConstType(const_type),
  mValue(value)
{
}

// @brief デストラクタ
EiBitVectorConst::~EiBitVectorConst()
{
}

// @brief 式のタイプを返す．
VlValueType
EiBitVectorConst::value_type() const
{
  ymuint size = mValue.size();
  bool sign = ( static_cast<int>(mConstType) & 8 ) == 8;
  return VlValueType(sign, true, size);
}

// @brief 定数の型を返す．
// @note 定数の時，意味を持つ．
tVpiConstType
EiBitVectorConst::constant_type() const
{
  return mConstType;
}

// @brief 定数値を返す．
// @note kVpiConstant の時，意味を持つ．
// @note それ以外では動作は不定
VlValue
EiBitVectorConst::constant_value() const
{
  return VlValue(mValue);
}


//////////////////////////////////////////////////////////////////////
// クラス EiRealConst
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] pt_expr パース木の定義要素
// @param[in] value 値
EiRealConst::EiRealConst(const PtExpr* pt_expr,
			 double value) :
  EiConstant(pt_expr),
  mValue(value)
{
}

// @brief デストラクタ
EiRealConst::~EiRealConst()
{
}

// @brief 式のタイプを返す．
VlValueType
EiRealConst::value_type() const
{
  return VlValueType::real_type();
}

// @brief 定数の型を返す．
// @note 定数の時，意味を持つ．
tVpiConstType
EiRealConst::constant_type() const
{
  return kVpiRealConst;
}

// @brief 定数値を返す．
// @note kVpiConstant の時，意味を持つ．
// @note それ以外では動作は不定
VlValue
EiRealConst::constant_value() const
{
  return VlValue(mValue);
}


//////////////////////////////////////////////////////////////////////
// クラス EiStringConst
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] pt_expr パース木の定義要素
// @param[in] value 値
EiStringConst::EiStringConst(const PtExpr* pt_expr,
			     const string& value) :
  EiConstant(pt_expr),
  mValue(value)
{
}

// @brief デストラクタ
EiStringConst::~EiStringConst()
{
}

// @brief 式のタイプを返す．
VlValueType
EiStringConst::value_type() const
{
  ymuint size = mValue.size();
  return VlValueType(false, true, size);
}

// @brief 定数の型を返す．
// @note 定数の時，意味を持つ．
tVpiConstType
EiStringConst::constant_type() const
{
  return kVpiStringConst;
}

// @brief 定数値を返す．
// @note kVpiConstant の時，意味を持つ．
// @note それ以外では動作は不定
VlValue
EiStringConst::constant_value() const
{
  return VlValue(mValue);
}

END_NAMESPACE_YM_VERILOG
