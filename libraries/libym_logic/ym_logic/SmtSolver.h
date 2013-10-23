#ifndef YM_LOGIC_SMTSOLVER_H
#define YM_LOGIC_SMTSOLVER_H

/// @file ym_logic/SmtSolver.h
/// @brief SmtSolver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#include "ym_logic/smt_nsdef.h"
#include "ym_utils/ShString.h"


BEGIN_NAMESPACE_YM_SMT

//////////////////////////////////////////////////////////////////////
/// @class SmtSolver SmtSolver.h "ym_logic/SmtSolver.h"
/// @brief SMT ソルバの雛形基底クラス
///
/// 実際にはここで定義されている仮想関数を実装したクラスを作る必要がある．
//////////////////////////////////////////////////////////////////////
class SmtSolver
{
public:

  /// @brief デストラクタ
  virtual
  ~SmtSolver() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが実装する仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 使用するロジックを設定する
  /// @param[in] logic 設定するロジック
  virtual
  tSmtLibResponse
  set_logic(tSmtLogic logic) = 0;

  /// @brief 型を作る．
  /// @param[in] elem_list 要素の型のリスト
  /// @return 作成した型を返す．
  virtual
  const SmtSort*
  make_sort(const vector<const SmtSort*>& elem_list) = 0;

  /// @brief 型テンプレート用のパラメータ型を作る．
  /// @param[in] param_id パラメータID
  /// @return 作成した型を返す．
  virtual
  const SmtSort*
  make_param_sort(ymuint param_id) = 0;

  /// @brief 変数を作る．
  /// @param[in] sort 変数の型
  /// @return 作成した変数を返す．
  virtual
  const SmtVar*
  make_var(const SmtSort* sort) = 0;

  /// @brief 関数を作る．
  /// @param[in] input_sort_list 入力の型のリスト
  /// @param[in] output_sort 出力の型
  /// @return 作成した関数を返す．
  virtual
  const SmtFun*
  make_fun(const vector<const SmtSort*>& input_sort_list,
	   const SmtSort* output_sort) = 0;

  /// @brief 内容を持った関数を作る．
  /// @param[in] input_var_list 入力の変数のリスト
  /// @param[in] body 本体を式
  /// @return 作成した関数を返す．
  virtual
  const SmtFun*
  make_fun(const vector<const SmtVar*>& input_var_list,
	   const SmtTerm* body) = 0;

  /// @brief <numeric> 型の term を作る．
  /// @param[in] val 値
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_numeric_term(ymuint32 val) = 0;

  /// @brief <decimal> 型の term を作る．
  /// @param[in] val 値
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_decimal_term(const ShString& val) = 0;

  /// @brief <hexadecimal> 型の term を作る．
  /// @param[in] val 値
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_hexadecimal_term(const ShString& val) = 0;

  /// @brief <binary> 型の term を作る．
  /// @param[in] val 値
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_binary_term(const ShString& val) = 0;

  /// @brief <string> 型の term を作る．
  /// @param[in] val 値
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_string_term(const ShString& val) = 0;

  /// @brief 変数型の term を作る．
  /// @param[in] var 変数
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_var_term(const SmtVar* var) = 0;

  /// @brief 関数呼び出しの term を作る．
  /// @param[in] fun 関数
  /// @param[in] arg_list 引数のリスト
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_fun_term(const SmtFun* fun,
		const vector<const SmtTerm*>& arg_list) = 0;

  /// @brief forall の term を作る．
  /// @param[in] var_list 束縛変数のリスト
  /// @param[in] body 本体の式
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_forall_term(const vector<const SmtVar*>& var_list,
		   const SmtTerm* body) = 0;

  /// @brief exists の term を作る．
  /// @param[in] var_list 束縛変数のリスト
  /// @param[in] body 本体の式
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_exists_term(const vector<const SmtVar*>& var_list,
		   const SmtTerm* body) = 0;

  /// @brief 属性付きの term を作る．
  /// @param[in] body 本体の式
  /// @param[in] attr_list 属性リスト
  /// @return 作成した式を返す．
  virtual
  const SmtTerm*
  make_annotated_term(const SmtTerm* body,
		      const vector<SmtAttr>& attr_list) = 0;

  /// @brief 充足可能性を調べる．
  /// @param[in] assert_list 仮定する式のリスト
  virtual
  tSmtLibResponse
  check_sat(const vector<const SmtTerm*>& assert_list) = 0;

  /// @brief 直前の check_sat() の証明を得る．
  /// @note check_sat() の結果が UNSAT の時のみ意味を持つ．
  virtual
  tSmtLibResponse
  get_proof() = 0;

  /// @brief 直前の check_sat() の unsat-core を得る．
  /// @param[out] term_list unsat-core の式を格納するリスト
  /// @note check_sat() の結果が UNSAT の時のみ意味を持つ．
  virtual
  tSmtLibResponse
  get_unsat_core(vector<const SmtTerm*>& term_list) = 0;

  /// @brief 直前の check_sat() の値割り当てを得る．
  /// @param[in] expr_list expr 式のリスト
  /// @param[out] value_list 値のリスト
  /// @note expr_list[i] に対応する値が value_list[i] に格納される．
  virtual
  tSmtLibResponse
  get_value(const vector<const SmtTerm*>& expr_list) = 0;

  /// @brief 直前の check_sat() の値割り当てを得る．
  virtual
  tSmtLibResponse
  get_assignment() = 0;

};

END_NAMESPACE_YM_SMT

#endif // YM_LOGIC_SMTSOLVER_H
