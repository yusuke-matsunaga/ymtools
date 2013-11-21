#ifndef LIBYM_VERILOG_ELABORATOR_INCLUDE_ELBPARAMASSIGN_H
#define LIBYM_VERILOG_ELABORATOR_INCLUDE_ELBPARAMASSIGN_H

/// @file libym_verilog/elaborator/include/ElbParamAssign.h
/// @brief ElbParamAssign のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_verilog/vl/VlParamAssign.h"


BEGIN_NAMESPACE_YM_VERILOG

//////////////////////////////////////////////////////////////////////
/// @class ElbParamAssign ElbParamAssign.h "ElbParamAssign.h"
/// @brief parameter 代入文を表すクラス
/// IEEE Std 1364-2001 26.6.12 Parameter, specparam
//////////////////////////////////////////////////////////////////////
class ElbParamAssign :
  public VlParamAssign
{
  friend class CellParamAssign;

protected:

  /// @brief コンストラクタ
  ElbParamAssign();

  /// @brief デストラクタ
  virtual
  ~ElbParamAssign();


public:
  //////////////////////////////////////////////////////////////////////
  // ElbParamAssign の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 次の要素を得る．
  const ElbParamAssign*
  next() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 次の要素を指すポインタ
  ElbParamAssign* mNext;

};


//////////////////////////////////////////////////////////////////////
/// @class ElbDefParam ElbParamAssign.h "ElbParamAssign.h"
/// @brief defparam 文を表すクラス
/// IEEE Std 1364-2001 26.6.12 Parameter, specparam
//////////////////////////////////////////////////////////////////////
class ElbDefParam :
  public VlDefParam
{
  friend class CellDefParam;

protected:

  /// @brief コンストラクタ
  ElbDefParam();

  /// @brief デストラクタ
  virtual
  ~ElbDefParam();


public:
  //////////////////////////////////////////////////////////////////////
  // ElbDefParam の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 次の要素を得る．
  const ElbDefParam*
  next() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 次の要素を指すポインタ
  ElbDefParam* mNext;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 次の要素を得る．
inline
const ElbParamAssign*
ElbParamAssign::next() const
{
  return mNext;
}

// @brief 次の要素を得る．
inline
const ElbDefParam*
ElbDefParam::next() const
{
  return mNext;
}

END_NAMESPACE_YM_VERILOG

#endif // LIBYM_VERILOG_ELABORATOR_INCLUDE_ELBPARAMASSIGN_H
