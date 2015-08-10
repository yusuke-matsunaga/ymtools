﻿#ifndef EXPRHANDLER_H
#define EXPRHANDLER_H

/// @file ExprHandler.h
/// @brief ExprHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "SimpleHandler.h"
#include "YmUtils/FileRegion.h"


BEGIN_NAMESPACE_YM_DOTLIB

//////////////////////////////////////////////////////////////////////
/// @class ExprHandler ExprHandler.h "ExprHandler.h"
/// @brief expression の値をとる simple attribute 用のハンドラ
//////////////////////////////////////////////////////////////////////
class ExprHandler :
  public SimpleHandler
{
public:

  /// @brief コンストラクタ
  /// @param[in] parent 親のハンドラ
  ExprHandler(GroupHandler* parent);

  /// @brief デストラクタ
  virtual
  ~ExprHandler();


protected:
  //////////////////////////////////////////////////////////////////////
  // SimpleHandler の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 値を読み込む処理
  /// @return 値を表す DotlibNode を返す．
  /// @note エラーが起きたら nullptr を返す．
  /// @note ここでは expression のパースを行う．
  virtual
  DotlibNodeImpl*
  read_value();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief primary を読み込む．
  DotlibNodeImpl*
  read_primary();

  /// @brief prudct を読み込む．
  DotlibNodeImpl*
  read_product();

  /// @brief expression を読み込む．
  DotlibNodeImpl*
  read_expr(tTokenType end_marker);

  /// @brief トークンを読み込む．
  /// @param[out] loc 対応するファイル上の位置情報を格納する変数
  tTokenType
  read_token(FileRegion& loc);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 読み戻したトークンの型
  tTokenType mUngetType;

  // 読み戻したトークンの位置
  FileRegion mUngetLoc;

};

END_NAMESPACE_YM_DOTLIB

#endif // EXPRHANDLER_H
