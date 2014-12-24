
/// @file AstImpl.cc
/// @brief AstImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "AstFuncCall.h"
#include "AstUniOp.h"
#include "AstBinOp.h"
#include "AstIteOp.h"
#include "AstVarExpr.h"
#include "AstIntConst.h"
#include "AstFloatConst.h"
#include "AstStringConst.h"
#include "AstValueType.h"

#include "AstFuncDecl.h"
#include "AstVarDecl.h"

#include "YmslCodeList.h"
#include "YmslVM.h"


BEGIN_NAMESPACE_YM_YMSL

#include "grammer.hh"


//////////////////////////////////////////////////////////////////////
// クラス AstExpr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] loc ファイル位置
AstExpr::AstExpr(const FileRegion& loc) :
  Ast(loc)
{
  mPrev = NULL;
}

// @brief デストラクタ
AstExpr::~AstExpr()
{
}

// @brief 前の要素を得る．
AstExpr*
AstExpr::prev() const
{
  return mPrev;
}

// @brief 前の要素をセットする．
void
AstExpr::set_prev(AstExpr* prev)
{
  mPrev = prev;
}


//////////////////////////////////////////////////////////////////////
// クラス AstFuncCall
//////////////////////////////////////////////////////////////////////

BEGIN_NONAMESPACE

ymuint
count_size(AstExpr* expr_list)
{
  ymuint n = 0;
  for (AstExpr* expr = expr_list; expr != NULL; expr = expr->prev()) {
    ++ n;
  }
  return n;
}

END_NONAMESPACE

// @brief コンストラクタ
// @param[in] func_name 関数名
// @param[in] expr_list 引数リスト
// @param[in] loc ファイル位置
AstFuncCall::AstFuncCall(AstSymbol* func_name,
			 AstExpr* expr_list,
			 const FileRegion& loc) :
  AstExpr(loc),
  mFuncName(func_name),
  mFunc(NULL),
  mExprList(count_size(expr_list))
{
  ymuint i = mExprList.size();
  for (AstExpr* expr = expr_list; expr != NULL; expr = expr->prev()) {
    -- i;
    mExprList[i] = expr;
  }
}

// @brief デストラクタ
AstFuncCall::~AstFuncCall()
{
}

// @brief 式の型を解析する．
// @return 引数の方が間違っていたら false を返す．
//
// 結果としてキャスト演算が挿入される場合もある．
bool
AstFuncCall::type_analysis()
{
  // 嘘
  return true;
}

// @brief 式の型を返す．
ValueType
AstFuncCall::type()
{
  return mFunc->type();
}

// @brief 命令コードのサイズを計算する．
ymuint
AstFuncCall::calc_size()
{
}

// @brief 命令コードを生成する．
// @param[in] driver ドライバ
// @param[in] code_list 命令コードの格納先
// @param[inout] addr 命令コードの現在のアドレス
//
// addr の値は更新される．
void
AstFuncCall::compile(YmslDriver& driver,
		     YmslCodeList& code_list,
		     Ymsl_INT& addr)
{
}

// @brief 内容を表示する．(デバッグ用)
// @param[in] s 出力ストリーム
void
AstFuncCall::print(ostream& s) const
{
  s << mFunc->name() << "(";
  {
    ymuint n = mExprList.size();
    const char* comma = "";
    for (ymuint i = 0; i < n; ++ i) {
      AstExpr* expr = mExprList[i];
      s << comma;
      comma = ", ";
      expr->print(s);
    }
  }
}


//////////////////////////////////////////////////////////////////////
// クラス AstBinOp
//////////////////////////////////////////////////////////////////////

// @breif コンストラクタ
// @param[in] token トークン
// @param[in] left, right オペランド
AstBinOp::AstBinOp(TokenType token,
		   AstExpr* left,
		   AstExpr* right) :
  AstExpr(FileRegion(left->file_region(), right->file_region())),
  mToken(token),
  mLeft(left),
  mRight(right)
{
}

// @brief デストラクタ
AstBinOp::~AstBinOp()
{
}

// @brief 式の型を解析する．
// @return 引数の方が間違っていたら false を返す．
//
// 結果としてキャスト演算が挿入される場合もある．
bool
AstBinOp::type_analysis()
{
}

// @brief 式の型を返す．
ValueType
AstBinOp::type()
{
  return mType;
}

// @brief 命令コードのサイズを計算する．
ymuint
AstBinOp::calc_size()
{
  return mLeft->calc_size() + mRight->calc_size() + 1;
}

// @brief 命令コードを生成する．
// @param[in] driver ドライバ
// @param[in] code_list 命令コードの格納先
// @param[inout] addr 命令コードの現在のアドレス
//
// addr の値は更新される．
void
AstBinOp::compile(YmslDriver& driver,
		  YmslCodeList& code_list,
		  Ymsl_INT& addr)
{
  mLeft->compile(driver, code_list, addr);
  mRight->compile(driver, code_list, addr);

  Ymsl_CODE op = YMVM_NOP;
  switch ( mType ) {
  case kVoidType:
    break;

  case kBooleanType:
    switch ( mToken) {
    case LOGAND: op = YMVM_INT_AND; break;
    case LOGOR:  op = YMVM_INT_OR; break;
    case EQEQ:   op = YMVM_INT_EQ; break;
    case NOTEQ:  op = YMVM_INT_NE; break;
    default: ASSERT_NOT_REACHED;
    }
    break;

  case kIntType:
    switch ( mToken ) {
    case PLUS:   op = YMVM_INT_ADD; break;
    case MINUS:  op = YMVM_INT_SUB; break;
    case MULT:   op = YMVM_INT_MUL; break;
    case DIV:    op = YMVM_INT_DIV; break;
    case MOD:    op = YMVM_INT_MOD; break;
    case BITAND: op = YMVM_INT_AND; break;
    case BITOR:  op = YMVM_INT_OR; break;
    case BITXOR: op = YMVM_INT_XOR; break;
    case EQEQ:   op = YMVM_INT_EQ; break;
    case NOTEQ:  op = YMVM_INT_NE; break;
    case LT:     op = YMVM_INT_LT; break;
    case GT:     op = YMVM_INT_GT; break;
    case LE:     op = YMVM_INT_LE; break;
    case GE:     op = YMVM_INT_GE; break;
    default: ASSERT_NOT_REACHED;
    }
    break;

  case kFloatType:
    switch ( mToken) {
    case PLUS:   op = YMVM_FLOAT_ADD; break;
    case MINUS:  op = YMVM_FLOAT_SUB; break;
    case MULT:   op = YMVM_FLOAT_MUL; break;
    case DIV:    op = YMVM_FLOAT_DIV; break;
    case EQEQ:   op = YMVM_FLOAT_EQ; break;
    case NOTEQ:  op = YMVM_FLOAT_NE; break;
    case LT:     op = YMVM_FLOAT_LT; break;
    case GT:     op = YMVM_FLOAT_GE; break;
    case LE:     op = YMVM_FLOAT_LE; break;
    case GE:     op = YMVM_FLOAT_GE; break;
    default: ASSERT_NOT_REACHED;
    }
    break;

  case kStringType:
    switch ( mToken) {
    case PLUS:
    case MULT:
    case EQEQ:
    case NOTEQ:
    case LT:
    case GT:
    case LE:
    case GE:
    default: ASSERT_NOT_REACHED;
    }
    break;

  case kUserType:
    break;
  }
  code_list.write_opcode(addr, op);
}

// @brief 内容を表示する．(デバッグ用)
// @param[in] s 出力ストリーム
void
AstBinOp::print(ostream& s) const
{
  s << "(";
  mLeft->print(s);
  switch ( mToken) {
  case PLUS:   s << "+"; break;
  case MINUS:  s << "-"; break;
  case MULT:   s << "*"; break;
  case DIV:    s << "/"; break;
  case MOD:    s << "%"; break;
  case BITAND: s << "&"; break;
  case BITOR:  s << "|"; break;
  case BITXOR: s << "^"; break;
  case LOGAND: s << " and "; break;
  case LOGOR:  s << " or "; break;
  case EQEQ:   s << "=="; break;
  case NOTEQ:  s << "!="; break;
  case LT:     s << "<"; break;
  case GT:     s << ">"; break;
  case LE:     s << "<="; break;
  case GE:     s << ">="; break;
  default: ASSERT_NOT_REACHED;
  }
  mRight->print(s);
  s << ")";
}


//////////////////////////////////////////////////////////////////////
// クラス AstIteOp
//////////////////////////////////////////////////////////////////////

// @breif コンストラクタ
// @param[in] opr1, opr2, opr3 オペランド
AstIteOp::AstIteOp(AstExpr* opr1,
		   AstExpr* opr2,
		   AstExpr* opr3) :
  AstExpr(FileRegion(opr1->file_region(), opr3->file_region()))
{
  mOpr[0] = opr1;
  mOpr[1] = opr2;
  mOpr[2] = opr3;
}

// @brief デストラクタ
AstIteOp::~AstIteOp()
{
}

// @brief 式の型を解析する．
// @return 引数の方が間違っていたら false を返す．
//
// 結果としてキャスト演算が挿入される場合もある．
bool
AstIteOp::type_analysis()
{
}

// @brief 式の型を返す．
ValueType
AstIteOp::type()
{
  return mType;
}

// @brief 命令コードのサイズを計算する．
ymuint
AstIteOp::calc_size()
{
}

// @brief 命令コードを生成する．
// @param[in] driver ドライバ
// @param[in] code_list 命令コードの格納先
// @param[inout] addr 命令コードの現在のアドレス
//
// addr の値は更新される．
void
AstIteOp::compile(YmslDriver& driver,
		  YmslCodeList& code_list,
		  Ymsl_INT& addr)
{
}

// @brief 内容を表示する．(デバッグ用)
// @param[in] s 出力ストリーム
void
AstIteOp::print(ostream& s) const
{
  mOpr[0]->print(s);
  s << " ? ";
  mOpr[1]->print(s);
  s << " : ";
  mOpr[2]->print(s);
}


//////////////////////////////////////////////////////////////////////
// クラス AstUniOp
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] token トークン
// @param[in] opr オペランド
// @param[in] loc ファイル位置
AstUniOp::AstUniOp(TokenType token,
		   AstExpr* opr,
		   const FileRegion& loc) :
  AstExpr(FileRegion(loc, opr->file_region())),
  mToken(token),
  mOperand(opr)
{
}

// @brief デストラクタ
AstUniOp::~AstUniOp()
{
}

// @brief 式の型を解析する．
// @return 引数の方が間違っていたら false を返す．
//
// 結果としてキャスト演算が挿入される場合もある．
bool
AstUniOp::type_analysis()
{
}

// @brief 式の型を返す．
ValueType
AstUniOp::type()
{
  return mType;
}

// @brief 命令コードのサイズを計算する．
ymuint
AstUniOp::calc_size()
{
  return mOperand->calc_size() + 1;
}

// @brief 命令コードを生成する．
// @param[in] driver ドライバ
// @param[in] code_list 命令コードの格納先
// @param[inout] addr 命令コードの現在のアドレス
//
// addr の値は更新される．
void
AstUniOp::compile(YmslDriver& driver,
		  YmslCodeList& code_list,
		  Ymsl_INT& addr)
{
  mOperand->compile(driver, code_list, addr);

  Ymsl_CODE op = YMVM_NOP;
  switch ( mType ) {
  case kVoidType:
    break;

  case kBooleanType:
    switch( mToken ) {
    case LOGNOT: op = YMVM_INT_NOT; break;
    default: ASSERT_NOT_REACHED;
    }
    break;

  case kIntType:
    switch ( mToken ) {
    case MINUS:  op = YMVM_INT_MINUS; break;
    case BITNEG: op = YMVM_INT_NOT; break;
    default: ASSERT_NOT_REACHED;
    }
    break;

  case kFloatType:
    switch ( mToken ) {
    case MINUS:  op = YMVM_FLOAT_MINUS; break;
    default: ASSERT_NOT_REACHED;
    }
    break;

  case kStringType:
    break;

  case kUserType:
    break;
  }
  code_list.write_opcode(addr, op);
}

// @brief 内容を表示する．(デバッグ用)
// @param[in] s 出力ストリーム
void
AstUniOp::print(ostream& s) const
{
  switch ( mToken ) {
  case PLUS:   s << "+"; break;
  case MINUS:  s << "-"; break;
  case BITNEG: s << "~"; break;
  case LOGNOT: s << "!"; break;
  default: ASSERT_NOT_REACHED;
  }
  mOperand->print(s);
}


//////////////////////////////////////////////////////////////////////
// クラス AstVarExpr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] var 変数
// @param[in] loc ファイル位置
AstVarExpr::AstVarExpr(AstVarDecl* var,
		       const FileRegion& loc) :
  AstExpr(loc),
  mVar(var)
{
}

// @brief デストラクタ
AstVarExpr::~AstVarExpr()
{
}

// @brief 式の型を解析する．
// @return 引数の方が間違っていたら false を返す．
//
// 結果としてキャスト演算が挿入される場合もある．
bool
AstVarExpr::type_analysis()
{
  return true;
}

// @brief 式の型を返す．
ValueType
AstVarExpr::type()
{
  return mVar->type();
}

// @brief 命令コードのサイズを計算する．
ymuint
AstVarExpr::calc_size()
{
  return 2;
}

// @brief 命令コードを生成する．
// @param[in] driver ドライバ
// @param[in] code_list 命令コードの格納先
// @param[inout] addr 命令コードの現在のアドレス
//
// addr の値は更新される．
void
AstVarExpr::compile(YmslDriver& driver,
		    YmslCodeList& code_list,
		    Ymsl_INT& addr)
{
  Ymsl_CODE op;
  switch ( mVar->type() ) {
  case kVoidType:
    break;

  case kBooleanType:
  case kIntType:
    if ( mVar->global() ) {
      op = YMVM_LOAD_GLOBAL_INT;
    }
    else {
      op = YMVM_LOAD_LOCAL_INT;
    }
    break;

  case kFloatType:
    if ( mVar->global() ) {
      op = YMVM_LOAD_GLOBAL_FLOAT;
    }
    else {
      op = YMVM_LOAD_LOCAL_FLOAT;
    }
    break;

  case kStringType:

    break;

  case kUserType:
    if ( mVar->global() ) {
      op = YMVM_LOAD_GLOBAL_OBJ;
    }
    else {
      op = YMVM_LOAD_LOCAL_OBJ;
    }
    break;
  }
  code_list.write_opcode(addr, op);
  code_list.write_int(addr, mVar->index());
}

// @brief 内容を表示する．(デバッグ用)
// @param[in] s 出力ストリーム
void
AstVarExpr::print(ostream& s) const
{
  s << mVar->name();
}


//////////////////////////////////////////////////////////////////////
// クラス AstIntConst
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] val 値
// @param[in] loc ファイル位置
AstIntConst::AstIntConst(int val,
			 const FileRegion& loc) :
  AstExpr(loc),
  mVal(val)
{
}

// @brief デストラクタ
AstIntConst::~AstIntConst()
{
}

// @brief 式の型を解析する．
// @return 引数の方が間違っていたら false を返す．
//
// 結果としてキャスト演算が挿入される場合もある．
bool
AstIntConst::type_analysis()
{
  return false;
}

// @brief 式の型を返す．
ValueType
AstIntConst::type()
{
  return kIntType;
}

// @brief 命令コードのサイズを計算する．
ymuint
AstIntConst::calc_size()
{
  return 2;
}

// @brief 命令コードを生成する．
// @param[in] driver ドライバ
// @param[in] code_list 命令コードの格納先
// @param[inout] addr 命令コードの現在のアドレス
//
// addr の値は更新される．
void
AstIntConst::compile(YmslDriver& driver,
		     YmslCodeList& code_list,
		     Ymsl_INT& addr)
{
  code_list.write_opcode(addr, YMVM_PUSH_INT_IMM);
  code_list.write_opcode(addr, mVal);
}

// @brief 内容を表示する．(デバッグ用)
// @param[in] s 出力ストリーム
void
AstIntConst::print(ostream& s) const
{
  s << mVal;
}


//////////////////////////////////////////////////////////////////////
// クラス AstFloatConst
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] val 値
// @param[in] loc ファイル位置
AstFloatConst::AstFloatConst(double val,
			     const FileRegion& loc) :
  AstExpr(loc),
  mVal(val)
{
}

// @brief デストラクタ
AstFloatConst::~AstFloatConst()
{
}

// @brief 式の型を解析する．
// @return 引数の方が間違っていたら false を返す．
//
// 結果としてキャスト演算が挿入される場合もある．
bool
AstFloatConst::type_analysis()
{
  return true;
}

// @brief 式の型を返す．
ValueType
AstFloatConst::type()
{
  return kFloatType;
}

// @brief 命令コードのサイズを計算する．
ymuint
AstFloatConst::calc_size()
{
  // 値によって異なる．
  if ( mVal == 0.0 || mVal == 1.0 ) {
    return 1;
  }
  else {
    return 1 + (sizeof(Ymsl_FLOAT) / sizeof(Ymsl_CODE));
  }
}

// @brief 命令コードを生成する．
// @param[in] driver ドライバ
// @param[in] code_list 命令コードの格納先
// @param[inout] addr 命令コードの現在のアドレス
//
// addr の値は更新される．
void
AstFloatConst::compile(YmslDriver& driver,
		       YmslCodeList& code_list,
		       Ymsl_INT& addr)
{
  if ( mVal == 0.0 ) {
    code_list.write_opcode(addr, YMVM_PUSH_FLOAT_ZERO);
  }
  else if ( mVal == 1.0 ) {
    code_list.write_opcode(addr, YMVM_PUSH_FLOAT_ONE);
  }
  else {
    code_list.write_opcode(addr, YMVM_PUSH_FLOAT_IMM);
    code_list.write_float(addr, mVal);
  }
}

// @brief 内容を表示する．(デバッグ用)
// @param[in] s 出力ストリーム
void
AstFloatConst::print(ostream& s) const
{
  s << mVal;
}


//////////////////////////////////////////////////////////////////////
// クラス AstStringConst
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] val 値
// @param[in] loc ファイル位置
AstStringConst::AstStringConst(const char* val,
			       const FileRegion& loc) :
  AstExpr(loc)
{
}

// @brief デストラクタ
AstStringConst::~AstStringConst()
{
}

// @brief 式の型を解析する．
// @return 引数の方が間違っていたら false を返す．
//
// 結果としてキャスト演算が挿入される場合もある．
bool
AstStringConst::type_analysis()
{
  return true;
}

// @brief 式の型を返す．
ValueType
AstStringConst::type()
{
  return kStringType;
}

// @brief 命令コードのサイズを計算する．
ymuint
AstStringConst::calc_size()
{
}

// @brief 命令コードを生成する．
// @param[in] driver ドライバ
// @param[in] code_list 命令コードの格納先
// @param[inout] addr 命令コードの現在のアドレス
//
// addr の値は更新される．
void
AstStringConst::compile(YmslDriver& driver,
			YmslCodeList& code_list,
			Ymsl_INT& addr)
{
}

// @brief 内容を表示する．(デバッグ用)
// @param[in] s 出力ストリーム
void
AstStringConst::print(ostream& s) const
{
  s << mVal;
}

END_NAMESPACE_YM_YMSL