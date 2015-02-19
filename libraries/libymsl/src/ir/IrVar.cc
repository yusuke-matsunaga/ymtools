
/// @file IrVar.cc
/// @brief IrVar の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "IrVar.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
// クラス IrVar
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] name 変数名
// @param[in] value_type 型
// @param[in] global グローバル変数の時 true にするフラグ
IrVar::IrVar(ShString name,
	     const Type* value_type,
	     bool global) :
  mName(name),
  mValueType(value_type),
  mGlobal(global),
  mIndex(0)
{
}

// @brief デストラクタ
IrVar::~IrVar()
{
}

// @brief 変数名を得る．
ShString
IrVar::name() const
{
  return mName;
}

// @brief 型を得る．
const Type*
IrVar::value_type() const
{
  return mValueType;
}

// @brief グローバル変数の時に true を返す．
bool
IrVar::is_global() const
{
  return mGlobal;
}

// @brief インデックスを返す．
ymuint
IrVar::index() const
{
  return mIndex;
}

// @brief インデックスを設定する．
void
IrVar::set_index(ymuint index)
{
  mIndex = index;
}

END_NAMESPACE_YM_YMSL