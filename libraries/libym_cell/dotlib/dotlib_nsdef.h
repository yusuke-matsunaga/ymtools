#ifndef LIBYM_CELL_DOTLIB_DOTLIB_NSDEF_H
#define LIBYM_CELL_DOTLIB_DOTLIB_NSDEF_H

/// @file libym_cell/dotlib/dotlib_nsdef.h
/// @brief dotlib サブモジュールの定義ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_cell/cell_nsdef.h"


//////////////////////////////////////////////////////////////////////
// 名前空間を定義するためのマクロ
//////////////////////////////////////////////////////////////////////

/// @brief dotlib の名前空間の開始
#define BEGIN_NAMESPACE_YM_CELL_DOTLIB \
BEGIN_NAMESPACE_YM_CELL \
BEGIN_NAMESPACE(nsDotlib)

/// @brief dotlib の名前空間の終了
#define END_NAMESPACE_YM_CELL_DOTLIB \
END_NAMESPACE(nsDotlib) \
END_NAMESPACE_YM_CELL


BEGIN_NAMESPACE_YM_CELL_DOTLIB

/// @brief トークンの値
enum tTokenType {
  /// @brief コロン(:)
  COLON,
  /// @brief セミコロン(;)
  SEMI,
  /// @brief コンマ(,)
  COMMA,
  /// @brief ハイフン(-)
  MINUS,
  /// @brief 左括弧( ( )
  LP,
  /// @brief 右括弧( ) )
  RP,
  /// @brief 左中括弧( { )
  LCB,
  /// @brief 右中括弧( } )
  RCB,
  /// @brief 整数値
  INT_NUM,
  /// @brief 実数値
  FLOAT_NUM,
  /// @brief 識別子
  ID_STR,
  /// @brief 文字列
  STR,
  /// @brief 改行
  NL,
  /// @brief エラー
  ERROR,
  /// @brief ファイルの末尾
  END
};

// クラス名の前方宣言
class DotLibParser;
class DotLibLex;
class DotLibHandler;

END_NAMESPACE_YM_CELL_DOTLIB


BEGIN_NAMESPACE_YM_CELL

using nsYm::nsCell::nsDotlib::DotLibParser;

END_NAMESPACE_YM_CELL

#endif // LIBYM_CELL_DOTLIB_DOTLIB_NSDEF_H