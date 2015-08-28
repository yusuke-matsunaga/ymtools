﻿
/// @file SatSolverMiniSat.cc
/// @brief SatSolverMiniSat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "SatSolverMiniSat.h"
#include "YmLogic/SatStats.h"


BEGIN_NAMESPACE_YM_SAT


//////////////////////////////////////////////////////////////////////
// SatSolverMiniSat
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] option オプション文字列
SatSolverMiniSat::SatSolverMiniSat(const string& option)
{
  if ( option == "verbose" ) {
    mSolver.verbosity = 1;
  }
}

// @brief デストラクタ
SatSolverMiniSat::~SatSolverMiniSat()
{
}

// @brief 正しい状態のときに true を返す．
bool
SatSolverMiniSat::sane() const
{
  return mSolver.okay();
}

// @brief 変数を追加する．
// @param[in] decision 決定変数の時に true とする．
// @return 新しい変数番号を返す．
// @note 変数番号は 0 から始まる．
VarId
SatSolverMiniSat::new_var(bool decision)
{
  return VarId(mSolver.newVar());
}

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
void
SatSolverMiniSat::add_clause(const vector<Literal>& lits)
{
  vec<Lit> tmp;
  for (vector<Literal>::const_iterator p = lits.begin();
       p != lits.end(); ++ p) {
    Literal l = *p;
    Lit lit(l.varid().val(), l.is_negative());
    tmp.push(lit);
  }
  mSolver.addClause(tmp);
}

// @brief 節を追加する．
// @param[in] lit_num リテラル数
// @param[in] lits リテラルの配列
void
SatSolverMiniSat::add_clause(ymuint lit_num,
			     const Literal* lits)
{
  vec<Lit> tmp;
  for (ymuint i = 0; i < lit_num; ++ i) {
    Literal l = lits[i];
    Lit lit(l.varid().val(), l.is_negative());
    tmp.push(lit);
  }
  mSolver.addClause(tmp);
}

// @brief SAT 問題を解く．
// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
// @param[out] model 充足するときの値の割り当てを格納する配列．
// @retval kB3True 充足した．
// @retval kB3False 充足不能が判明した．
// @retval kB3X わからなかった．
// @note i 番めの変数の割り当て結果は model[i] に入る．
Bool3
SatSolverMiniSat::solve(const vector<Literal>& assumptions,
			vector<Bool3>& model)
{
  vec<Lit> tmp;
  for (vector<Literal>::const_iterator p = assumptions.begin();
       p != assumptions.end(); ++ p) {
    Literal l = *p;
    Lit lit(l.varid().val(), l.is_negative());
    tmp.push(lit);
  }
  bool ans = mSolver.solve(tmp);
  if ( ans ) {
    ymuint n = mSolver.model.size();
    model.resize(n);
    for (ymuint i = 0; i < n; ++ i) {
      lbool lb = mSolver.model[i];
      if ( lb == l_True ) {
	model[i] = kB3True;
      }
      else if ( lb == l_False ) {
	model[i] = kB3False;
      }
      else {
	model[i] = kB3X;
      }
    }
    return kB3True;
  }
  return kB3False;
}

// @brief 探索を中止する．
//
// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
void
SatSolverMiniSat::stop()
{
  // 未実装
}

// @brief 学習節をすべて削除する．
void
SatSolverMiniSat::forget_learnt_clause()
{
  // 未実装
}

// @brief conflict_limit の最大値
// @param[in] val 設定する値
// @return 以前の設定値を返す．
ymuint64
SatSolverMiniSat::set_max_conflict(ymuint64 val)
{
  // 無効
  return 0;
}

// @brief 現在の内部状態を得る．
// @param[out] stats 状態を格納する構造体
void
SatSolverMiniSat::get_stats(SatStats& stats) const
{
  stats.mRestart = mSolver.stats.starts;
  stats.mVarNum = mSolver.nVars();
  stats.mConstrClauseNum = mSolver.nClauses();
  stats.mConstrLitNum = mSolver.stats.clauses_literals;
  stats.mLearntClauseNum = mSolver.nLearnts();
  stats.mLearntLitNum = mSolver.stats.learnts_literals;
  stats.mConflictNum = mSolver.stats.conflicts;
  stats.mDecisionNum = mSolver.stats.decisions;
  stats.mPropagationNum = mSolver.stats.propagations;
  stats.mConflictLimit = 0;
  stats.mLearntLimit = 0;
}

// @brief 変数の数を得る．
ymuint
SatSolverMiniSat::variable_num() const
{
  return mSolver.nVars();
}

// @brief 制約節の数を得る．
ymuint
SatSolverMiniSat::clause_num() const
{
  return mSolver.nClauses();
}

// @brief 制約節のリテラルの総数を得る．
ymuint
SatSolverMiniSat::literal_num() const
{
  return mSolver.stats.clauses_literals;
}

// @brief DIMACS 形式で制約節を出力する．
// @param[in] s 出力先のストリーム
void
SatSolverMiniSat::write_DIMACS(ostream& s) const
{
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
// @param[in] msg_handler 登録するメッセージハンドラ
void
SatSolverMiniSat::reg_msg_handler(SatMsgHandler* msg_handler)
{
}

// @brief 時間計測機能を制御する
void
SatSolverMiniSat::timer_on(bool enable)
{
}

END_NAMESPACE_YM_SAT
