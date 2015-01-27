#ifndef YMSLCOMPILER_H
#define YMSLCOMPILER_H

/// @file YmslCompiler.h
/// @brief YmslCompiler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "ymsl_int.h"
#include "IrMgr.h"
#include "IrTypeMgr.h"
#include "YmUtils/IDO.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
/// @class YmslCompiler YmslCompiler.h "YmslCompiler.h"
/// @brief YMSL 用のコンパイラ
//////////////////////////////////////////////////////////////////////
class YmslCompiler
{
public:

  /// @brief コンストラクタ
  YmslCompiler();

  /// @brief デストラクタ
  ~YmslCompiler();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief コンパイルする．
  /// @param[in] ido 入力データ
  /// @return 成功したら true を返す．
  bool
  compile(IDO& ido);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素の生成を行う．
  /// @param[in] stmt 文
  /// @param[in] scope 現在のスコープ
  void
  phase1(const AstStatement* stmt,
	 IrScope* scope);

  /// @brief enum 型の定義を行う．
  /// @param[in] stmt 文
  /// @param[in] scope 現在のスコープ
  ///
  /// stmt は kEnumDecl でなければならない．
  void
  reg_enum(const AstStatement* stmt,
	   IrScope* scope);

  /// @brief 関数の定義を行う．
  /// @param[in] stmt 文
  /// @param[in] scope 現在のスコープ
  ///
  /// stmt は kFuncDecl でなければならない．
  void
  reg_func(const AstStatement* stmt,
	   IrScope* scope);

  /// @brief 変数の定義を行う．
  /// @param[in] stmt 文
  /// @param[in] scope 現在のスコープ
  ///
  /// stmt は kVarDecl でなければならない．
  void
  reg_var(const AstStatement* stmt,
	  IrScope* scope);

  /// @brief 型の参照を解決する．
  /// @param[in] asttype 型を表す構文木
  /// @param[in] scope 現在のスコープ
  ///
  /// 解決できない時には NULL を返す．
  const IrType*
  resolve_type(const AstType* asttype,
	       IrScope* scope);

  /// @brief 参照の解決を行う．
  /// @param[in] stmt 文
  /// @param[in] scope 現在のスコープ
  void
  phase2(const AstStatement* stmt,
	 IrScope* scope);

  /// @brief 式の実体化を行う．
  /// @param[in] ast_expr 式を表す構文木
  /// @param[in] scope 現在のスコープ
  IrExpr*
  elab_expr(const AstExpr* ast_expr,
	    IrScope* scope);

  /// @brief シンボルに対応する式を返す．
  /// @param[in] symbol シンボル
  /// @param[in] scope スコープ
  IrExpr*
  symbol2expr(const AstSymbol* symbol,
	      IrScope* scope);

  /// @brief 変数を生成する．
  /// @param[in] name 名前
  /// @param[in] type 型
  IrVar*
  new_var(ShString name,
	  const IrType* type);

  /// @brief 関数を生成する．
  /// @param[in] name 名前
  /// @param[in] type 出力の型
  /// @param[in] input_type_list 入力の型のリスト
  IrFunction*
  new_function(ShString name,
	       const IrType* type,
	       const vector<const IrType*>& input_type_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // IR を管理するマネージャ
  IrMgr mIrMgr;

  // 型を管理するマネージャ
  IrTypeMgr mTypeMgr;

  // 文とスコープのリスト
  vector<pair<const AstStatement*, IrScope*> > mStmtList;

  // 変数のリスト
  vector<IrVar*> mVarList;

  // 関数のリスト
  vector<IrFunction*> mFuncList;

};

END_NAMESPACE_YM_YMSL


#endif // YMSLCOMPILER_H