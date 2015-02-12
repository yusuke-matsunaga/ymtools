#ifndef IRFUNCTION_H
#define IRFUNCTION_H

/// @file IrFunction.h
/// @brief IrFunction のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "IrCodeBlock.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
/// @class IrFunction IrFunction.h "IrFunction.h"
/// @brief 関数の中間表現
//////////////////////////////////////////////////////////////////////
class IrFunction :
  public IrCodeBlock
{
public:

  /// @brief コンストラクタ
  /// @param[in] toplevel トップレベルブロック
  /// @param[in] func 関数本体
  /// @param[in] arg_list 引数のリスト
  /// @param[in] arg_init_list 引数のデフォルト値のリスト
  IrFunction(IrToplevel& toplevel,
	     const Function* func,
	     const vector<const Var*>& arg_list,
	     const vector<IrNode*>& arg_init_list);

  /// @brief デストラクタ
  ~IrFunction();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief トップレベルのブロックを返す．
  virtual
  IrToplevel&
  toplevel();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // トップレベルブロック
  IrToplevel& mToplevel;

  // 関数本体
  const Function* mFunction;

  // 引数のリスト
  vector<const Var*> mArgList;

  // 引数のデフォルト値のリスト
  vector<IrNode*> mArgInitList;

};

END_NAMESPACE_YM_YMSL

#endif // IRFUNCTION_H
