
/// @file libym_logic/expr/LogExprWriter.cc
/// @brief LogExprWriter の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: LogExprWriter.cc 958 2007-08-28 05:38:02Z matsunaga $
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_logic/LogExprWriter.h"
#include "ym_logic/LogExpr.h"


BEGIN_NAMESPACE_YM_LEXP

// @brief コンストラクタ
LogExprWriter::LogExprWriter()
{
  mOpStrArray[0] = "~";
  mOpStrArray[1] = "&";
  mOpStrArray[2] = "|";
  mOpStrArray[3] = "^";
}

// @brief デストラクタ
LogExprWriter::~LogExprWriter()
{
}

// ostream に対する書出し
ostream&
LogExprWriter::dump(ostream& s,
		    const LogExpr& expr,
		    const VarStrMap& var_names) const
{
  dump_sub(s, expr, var_names);
  return s;
}

// @brief 内容を文字列にする．
// @param[in] expr 文字列
// @param[in] var_names 各変数番号から変数名への写像
// @note varid 番目の変数名として var_names[varid] を用いる．
// 登録されていなければデフォルトの表記を用いる．
string
LogExprWriter::dump_string(const LogExpr& expr,
			   const VarStrMap& var_names) const
{
  ostringstream buf;
  dump(buf, expr, var_names);
  return string(buf.str());
}

// 演算子文字列を設定する．
void
LogExprWriter::set_opstr(const vector<string>& op)
{
  for (ymuint i = 0; i < 4; i ++) {
    mOpStrArray[i] = op[i];
  }
}

// 演算子文字列を取得する．
void
LogExprWriter::opstr(vector<string>& op) const
{
  op.resize(4);
  for (ymuint i = 0; i < 4; i ++) {
    op[i] = mOpStrArray[i];
  }
}

// NOT演算子の文字列を得る．
const string&
LogExprWriter::not_str() const
{
  return mOpStrArray[0];
}

// AND演算子の文字列を得る．
const string&
LogExprWriter::and_str() const
{
  return mOpStrArray[1];
}

// OR演算子の文字列を得る．
const string&
LogExprWriter::or_str() const
{
  return mOpStrArray[2];
}

// XOR演算子の文字列を得る．
const string&
LogExprWriter::xor_str() const
{
  return mOpStrArray[3];
}

// 論理式の内容を ostream に書出す．
ostream&
operator<<(ostream& s,
	   const LogExpr& expr)
{
  LogExprWriter writer;
  // 空の map を渡す
  return writer.dump(s, expr, VarStrMap());
}


// dump() のサブルーティン
void
LogExprWriter::dump_sub(ostream& s,
			const LogExpr& expr,
			const VarStrMap& var_names) const
{
  if ( expr.is_zero() ) {
    s << "0";
  }
  else if ( expr.is_one() ) {
    s << "1";
  }
  else if ( expr.is_literal() ) {
    if ( expr.is_negaliteral() ) {
      s << not_str();
    }
    VarId id = expr.varid();
    string name;
    if ( var_names.find(id, name) ) {
      s << name;
    }
    else {
      s << "V" << id;
    }
  }
  else { // AND/OR/XOR
    string delim;
    if ( expr.is_and() ) {
      delim = and_str();
    }
    else if ( expr.is_or() ) {
      delim = or_str();
    }
    else if ( expr.is_xor() ) {
      delim = xor_str();
    }
    else {
      assert_not_reached(__FILE__, __LINE__);
    }
    s << "( ";
    string delim1 = "";
    ymuint n = expr.child_num();
    for (ymuint i = 0; i < n; ++ i) {
      s << delim1;
      dump_sub(s, expr.child(i), var_names);
      delim1 = " " + delim + " ";
    }
    s << " )";
  }
}

END_NAMESPACE_YM_LEXP
