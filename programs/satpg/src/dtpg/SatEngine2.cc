
/// @file SatEngine2.cc
/// @brief SatEngine2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014 Yusuke Matsunaga
/// All rights reserved.


#include "SatEngine2.h"

#include "DetectOp.h"
#include "UntestOp.h"
#include "DtpgStats.h"
#include "TpgNode.h"
#include "TpgPrimitive.h"
#include "TpgFault.h"
#include "BackTracer.h"
#include "InputLiteral.h"
#include "logic/SatSolver.h"
#include "logic/SatStats.h"


BEGIN_NAMESPACE_YM_SATPG

BEGIN_NONAMESPACE

// @brief 故障挿入回路を表す CNF 式を作る．
// @param[in] solver SAT ソルバー
// @param[in] ivar 入力の変数
// @param[in] fvar 故障変数
// @param[in] ovar 出力の変数
// @param[in] fval 故障値
void
make_flt_cnf(SatSolver& solver,
	     VarId ovar,
	     int vval)
{
  Literal l2(ovar, false);
  if ( vval == 0 ) {
    solver.add_clause(~l2);
  }
  else {
    solver.add_clause(l2);
  }
}

// @brief ノードに正常回路用の変数を設定する．
// @param[in] solver SAT ソルバー
// @param[in] node 対象のノード
void
set_gvar(SatSolver& solver,
	 TpgNode* node)
{
  // ノードそのものに割り当てる．
  VarId gvar = solver.new_var();
  node->set_gvar(gvar);
}

// @brief ノードに正常回路用の変数を設定する．
// @param[in] solver SAT ソルバー
// @param[in] node 対象のノード
void
set_fvar(SatSolver& solver,
	 TpgNode* node)
{
  // ノードそのものに割り当てる．
  VarId fvar = solver.new_var();
  VarId dvar = solver.new_var();
  node->set_fvar(fvar, dvar);
}

// @brief ゲートの入出力関係を表す CNF を作る．
// @param[in] solver SAT ソルバー
// @param[in] gate_type ゲートタイプ
// @param[in] output 出力のリテラル
// @param[in] inputs 入力のリテラル
void
make_gate_cnf(SatSolver& solver,
	      tTgGateType gate_type,
	      Literal output,
	      const InputLiteral& inputs)
{
  ymuint ni = inputs.size();
  switch ( gate_type ) {
  case kTgGateBuff:
    solver.add_clause( inputs[0], ~output);
    solver.add_clause(~inputs[0],  output);
    break;

  case kTgGateNot:
    solver.add_clause( inputs[0],  output);
    solver.add_clause(~inputs[0], ~output);
    break;

  case kTgGateAnd:
    switch ( ni ) {
    case 2:
      solver.add_clause(~inputs[0], ~inputs[1], output);
      break;

    case 3:
      solver.add_clause(~inputs[0], ~inputs[1], ~inputs[2], output);
      break;

    case 4:
      solver.add_clause(~inputs[0], ~inputs[1], ~inputs[2], ~inputs[3], output);
      break;

    default:
      {
	vector<Literal> tmp(ni + 1);
	for (ymuint i = 0; i < ni; ++ i) {
	  tmp[i] = ~inputs[i];
	}
	tmp[ni] = output;
	solver.add_clause(tmp);
      }
      break;
    }
    for (ymuint i = 0; i < ni; ++ i) {
      solver.add_clause(inputs[i], ~output);
    }
    break;

  case kTgGateNand:
    switch ( ni ) {
    case 2:
      solver.add_clause(~inputs[0], ~inputs[1], ~output);
      break;

    case 3:
      solver.add_clause(~inputs[0], ~inputs[1], ~inputs[2], ~output);
      break;

    case 4:
      solver.add_clause(~inputs[0], ~inputs[1], ~inputs[2], ~inputs[3], ~output);
      break;

    default:
      {
	vector<Literal> tmp(ni + 1);
	for (ymuint i = 0; i < ni; ++ i) {
	  tmp[i] = ~inputs[i];
	}
	tmp[ni] = ~output;
	solver.add_clause(tmp);
      }
      break;
    }
    for (ymuint i = 0; i < ni; ++ i) {
      solver.add_clause(inputs[i], output);
    }
    break;

  case kTgGateOr:
    switch ( ni ) {
    case 2:
      solver.add_clause(inputs[0], inputs[1], ~output);
      break;

    case 3:
      solver.add_clause(inputs[0], inputs[1], inputs[2], ~output);
      break;

    case 4:
      solver.add_clause(inputs[0], inputs[1], inputs[2], inputs[3], ~output);
      break;

    default:
      {
	vector<Literal> tmp(ni + 1);
	for (ymuint i = 0; i < ni; ++ i) {
	  tmp[i] = inputs[i];
	}
	tmp[ni] = ~output;
	solver.add_clause(tmp);
      }
      break;
    }
    for (ymuint i = 0; i < ni; ++ i) {
      solver.add_clause(~inputs[i], output);
    }
    break;

  case kTgGateNor:
    switch ( ni ) {
    case 2:
      solver.add_clause(inputs[0], inputs[1], output);
      break;

    case 3:
      solver.add_clause(inputs[0], inputs[1], inputs[2], output);
      break;

    case 4:
      solver.add_clause(inputs[0], inputs[1], inputs[2], inputs[3], output);
      break;

    default:
      {
	vector<Literal> tmp(ni + 1);
	for (ymuint i = 0; i < ni; ++ i) {
	  tmp[i] = inputs[i];
	}
	tmp[ni] = output;
	solver.add_clause(tmp);
      }
      break;
    }
    for (ymuint i = 0; i < ni; ++ i) {
      solver.add_clause(~inputs[i], ~output);
    }
    break;

  case kTgGateXor:
    if ( ni == 2 ) {
      solver.add_clause(~inputs[0],  inputs[1],  output);
      solver.add_clause( inputs[0], ~inputs[1],  output);
      solver.add_clause( inputs[0],  inputs[1], ~output);
      solver.add_clause(~inputs[0], ~inputs[1], ~output);
    }
    else {
      vector<Literal> tmp(ni + 1);
      ymuint nip = (1U << ni);
      for (ymuint p = 0; p < nip; ++ p) {
	ymuint c = 0;
	for (ymuint i = 0; i < ni; ++ i) {
	  if ( p & (1U << i) ) {
	    tmp[i] = inputs[i];
	  }
	  else {
	    tmp[i] = ~inputs[i];
	    ++ c;
	  }
	}
	if ( (c % 2) == 0 ) {
	  tmp[ni] = ~output;
	}
	else {
	  tmp[ni] = output;
	}
	solver.add_clause(tmp);
      }
    }
    break;

  case kTgGateXnor:
    if ( ni == 2 ) {
      solver.add_clause(~inputs[0],  inputs[1], ~output);
      solver.add_clause( inputs[0], ~inputs[1], ~output);
      solver.add_clause( inputs[0],  inputs[1],  output);
      solver.add_clause(~inputs[0], ~inputs[1],  output);
    }
    else {
      vector<Literal> tmp(ni + 1);
      ymuint nip = (1U << ni);
      for (ymuint p = 0; p < nip; ++ p) {
	ymuint c = 0;
	for (ymuint i = 0; i < ni; ++ i) {
	  if ( p & (1U << i) ) {
	    tmp[i] = inputs[i];
	  }
	  else {
	    tmp[i] = ~inputs[i];
	    ++ c;
	  }
	}
	if ( (c % 2) == 0 ) {
	  tmp[ni] = output;
	}
	else {
	  tmp[ni] = ~output;
	}
	solver.add_clause(tmp);
      }
    }
    break;

  default:
    assert_not_reached(__FILE__, __LINE__);
    break;
  }
}

// @brief 正常回路におけるノードの入出力の関係を表す CNF を作る．
void
make_gnode_cnf(SatSolver& solver,
	       TpgNode* node)
{
  if ( node->is_input() ) {
    return;
  }

  Literal output(node->gvar(), false);

  if ( node->is_output() ) {
    TpgNode* inode = node->fanin(0);
    Literal input(inode->gvar(), false);
    solver.add_clause( input, ~output);
    solver.add_clause(~input,  output);
    return;
  }

  if ( node->is_cplx_logic() ) {
    // 複数のプリミティブで構成されたノードの場合
    ymuint n = node->primitive_num();
    for (ymuint i = 0; i < n; ++ i) {
      TpgPrimitive* prim = node->primitive(i);
      Literal olit;
      if ( prim->is_input() ) {
	// 入力プリミティブの場合
	// 対応する TpgNode の変数を持ってくる．
	ymuint ipos = prim->input_id();
	TpgNode* inode = node->fanin(ipos);
	olit = Literal(inode->gvar(), false);;
      }
      else if ( prim->is_not_input() ) {
	// 否定付き入力プリミティブの場合
	// 対応する TpgNode の変数を持ってきて否定する．
	ymuint ipos = prim->input_id();
	TpgNode* inode = node->fanin(ipos);
	olit = Literal(inode->gvar(), true);
      }
      else {
	if ( i == n - 1 ) {
	  // 根のプリミティブの場合
	  // node の変数を使う．
	  olit = Literal(node->gvar(), false);
	}
	else {
	  // それ以外の場合
	  // 新たな変数を割り当てる．
	  olit = Literal(solver.new_var(), false);
	}

	// プリミティブの入出力の関係を表す CNF 式を作る．
	make_gate_cnf(solver, prim->gate_type(), olit,
		      PrimGvarInputLiteral(prim));
      }
      // prim の glit と flit の両方に登録しておく
      prim->set_glit(olit);
      prim->set_flit(olit);
    }
    return;
  }

  // 単純な組み込み型の場合
  make_gate_cnf(solver, node->gate_type(), output,
		GvarInputLiteral(node));
}

void
make_fnode_cnf(SatSolver& solver,
	       TpgNode* node,
	       Literal output,
	       const vector<Literal>& inputs)
{
  if ( node->is_input() ) {
    return;
  }

  if ( node->is_output() ) {
    solver.add_clause( inputs[0], ~output);
    solver.add_clause(~inputs[0],  output);
    return;
  }

  if ( node->is_cplx_logic() ) {
    ymuint n = node->primitive_num();
    ymuint n1 = n - 1;
    for (ymuint i = 0; i < n; ++ i) {
      TpgPrimitive* prim = node->primitive(i);
      if ( !prim->is_input() ) {
	ymuint ni1 = prim->fanin_num();
	vector<Literal> inputs1(ni1);
	for (ymuint j = 0; j < ni1; ++ j) {
	  const TpgPrimitive* iprim = prim->fanin(j);
	  if ( iprim->is_input() ) {
	    ymuint ipos = iprim->input_id();
	    inputs1[j] = inputs[ipos];
	  }
	  else if ( iprim->is_not_input() ) {
	    ymuint ipos = iprim->input_id();
	    inputs1[j] = ~inputs[ipos];
	  }
	  else {
	    inputs1[j] = iprim->flit();
	  }
	}
	Literal output1;
	if ( i == n1 ) {
	  output1 = output;
	}
	else {
	  output1 = prim->flit();
	}
	make_gate_cnf(solver, prim->gate_type(), output1,
		      VectorInputLiteral(inputs1));
      }
    }
    return;
  }

  // 残りは単純なゲート
  make_gate_cnf(solver, node->gate_type(), output,
		VectorInputLiteral(inputs));
}

END_NONAMESPACE


// @brief コンストラクタ
SatEngine2::SatEngine2()
{
  mTimerEnable = false;
}

// @brief デストラクタ
SatEngine2::~SatEngine2()
{
}

// @brief 使用する SAT エンジンを指定する．
void
SatEngine2::set_mode(const string& type,
		     const string& option,
		     ostream* outp)
{
  mType = type;
  mOption = option;
  mOutP = outp;
}

// @brief 統計情報をクリアする．
void
SatEngine2::clear_stats()
{
  mRunCount = 0;
  mSatCount = 0;
  mRestart = 0;
  mVarNum = 0;
  mConstrClauseNum = 0;
  mConstrLitNum = 0;
  mLearntClauseNum = 0;
  mLearntLitNum = 0;
  mConflictNum = 0;
  mDecisionNum = 0;
  mPropagationNum = 0;

  mCnfCount = 0;
  mCnfTime.set(0.0, 0.0, 0.0);
  mDetCount = 0;
  mDetTime.set(0.0, 0.0, 0.0);
  mUndetCount = 0;
  mUndetTime.set(0.0, 0.0, 0.0);
  mAbortCount = 0;
  mAbortTime.set(0.0, 0.0, 0.0);
}

// @brief テストパタン生成を行なう．
// @param[in] flist 故障リスト
// @param[in] max_id ノード番号の最大値 + 1
void
SatEngine2::run(TpgFault* f_tgt,
		ymuint max_id,
		BackTracer& bt,
		DetectOp& dop,
		UntestOp& uop)
{
  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }

  SatSolver solver(mType, mOption, mOutP);

  mMarkArray.clear();
  mMarkArray.resize(max_id, 0U);

  vector<TpgNode*> tfo_list;
  tfo_list.reserve(max_id);

  vector<TpgNode*> tfi_list;
  tfi_list.reserve(max_id);

  // 故障のあるノードの TFO を tfo_list に入れる．
  // TFO の TFI のノードを tfi_list に入れる．
  TpgNode* fnode = f_tgt->node();
  if ( !tfo_mark(fnode) ) {
    set_tfo_mark(fnode);
    tfo_list.push_back(fnode);
  }

  for (ymuint rpos = 0; rpos < tfo_list.size(); ++ rpos) {
    TpgNode* node = tfo_list[rpos];
    ymuint nfo = node->active_fanout_num();
    for (ymuint i = 0; i < nfo; ++ i) {
      TpgNode* fonode = node->active_fanout(i);
      if ( !tfo_mark(fonode) ) {
	set_tfo_mark(fonode);
	tfo_list.push_back(fonode);
      }
    }
  }

  for (vector<TpgNode*>::iterator p = tfo_list.begin();
       p != tfo_list.end(); ++ p) {
    TpgNode* node = *p;
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      TpgNode* finode = node->fanin(i);
      if ( !tfo_mark(finode) && !tfi_mark(finode) ) {
	set_tfi_mark(finode);
	tfi_list.push_back(finode);
      }
    }
  }
  for (ymuint rpos = 0; rpos < tfi_list.size(); ++ rpos) {
    TpgNode* node = tfi_list[rpos];
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      TpgNode* finode = node->fanin(i);
      if ( !tfo_mark(finode) && !tfi_mark(finode) ) {
	set_tfi_mark(finode);
	tfi_list.push_back(finode);
      }
    }
  }

  // TFO マークのついたノード用の変数の生成
  mUsedNodeList.clear();
  mUsedNodeList.reserve(tfo_list.size() + tfi_list.size());
  for (vector<TpgNode*>::iterator p = tfo_list.begin();
       p != tfo_list.end(); ++ p) {
    TpgNode* node = *p;
    set_gvar(solver, node);
    set_fvar(solver, node);
    mUsedNodeList.push_back(node);
  }

  // TFI マークのついたノード用の変数の生成
  for (vector<TpgNode*>::iterator p = tfi_list.begin();
       p != tfi_list.end(); ++ p) {
    TpgNode* node = *p;
    set_gvar(solver, node);
    mUsedNodeList.push_back(node);
  }

  // mInputList を作る．
  mInputList.clear();
  for (vector<TpgNode*>::iterator p = tfi_list.begin();
       p != tfi_list.end(); ++ p) {
    TpgNode* node = *p;
    if ( node->is_input() ) {
      mInputList.push_back(node);
    }
  }

  // mOutputList を作る．
  mOutputList.clear();
  for (vector<TpgNode*>::iterator p = tfo_list.begin();
       p != tfo_list.end(); ++ p) {
    TpgNode* node = *p;
    if ( node->is_input() ) {
      mInputList.push_back(node);
    }
    if ( node->is_output() ) {
      mOutputList.push_back(node);
    }
  }


  //////////////////////////////////////////////////////////////////////
  // 正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////

  for (vector<TpgNode*>::const_iterator p = tfi_list.begin();
       p != tfi_list.end(); ++ p) {
    TpgNode* node = *p;
    make_gnode_cnf(solver, node);
  }
  for (vector<TpgNode*>::const_iterator p = tfo_list.begin();
       p != tfo_list.end(); ++ p) {
    TpgNode* node = *p;
    make_gnode_cnf(solver, node);
  }


  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  vector<Literal> dep;
  for (ymuint i = 0; i < tfo_list.size(); ++ i) {
    TpgNode* node = tfo_list[i];

    bool has_fault = f_tgt->node() == node;

    // inputs[] に入力変数を表すリテラルを格納する．
    // ただし，入力の故障を仮定する場合には故障挿入回路の出力変数となる．
    ymuint ni = node->fanin_num();
    vector<Literal> inputs(ni);
    for (ymuint i = 0; i < ni; ++ i) {
      TpgNode* inode = node->fanin(i);
      VarId ivar = inode->fvar();
      inputs[i] = Literal(ivar, false);
    }

    // ovar に出力変数を入れる．
    // こちらは入力の故障と異なり，故障挿入回路の出力が node->fvar() となる．
    // 逆に ovar はゲートの直接の出力変数となる．
    VarId ovar = node->fvar();

    if ( has_fault ) {
      if ( f_tgt->is_input_fault() ) {
	VarId fvar = solver.new_var();
	make_flt_cnf(solver, fvar, f_tgt->val());
	inputs[f_tgt->pos()] = Literal(fvar, false);
      }
      else {
	VarId fvar = node->fvar();
	make_flt_cnf(solver, fvar, f_tgt->val());
	// ダミー
	ovar = solver.new_var();
      }
    }

    Literal gate_output(ovar, false);
    make_fnode_cnf(solver, node, gate_output, inputs);

    Literal glit(node->gvar(), false);
    Literal flit(node->fvar(), false);
    Literal dlit(node->dvar(), false);

    // XOR(glit, flit, dlit) を追加する．
    // 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
    solver.add_clause(~glit, ~flit, ~dlit);
    solver.add_clause(~glit,  flit,  dlit);
    solver.add_clause( glit, ~flit,  dlit);
    solver.add_clause( glit,  flit, ~dlit);

    // 出力の dlit が1になる条件を作る．
    // - 1) 入力の dlit のいずれかが 1
    // - 2) 入力のいずれかに故障がある．
    // - 3) 出力に故障がある．
    // このうち，2) と 3) の場合は has_fault = true となっている．
    if ( !has_fault ) {
      dep.clear();
      dep.reserve(ni + 1);
      Literal dlit(node->dvar(), true);
      dep.push_back(dlit);
      for (ymuint j = 0; j < ni; ++ j) {
	TpgNode* inode = node->fanin(j);
	if ( inode->has_fvar() ) {
	  dep.push_back(Literal(inode->dvar(), false));
	}
      }

      solver.add_clause(dep);
    }
  }


  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件
  //////////////////////////////////////////////////////////////////////
  vector<Literal> odiff;
  odiff.reserve(mOutputList.size());
  for (vector<TpgNode*>::iterator p = mOutputList.begin();
       p != mOutputList.end(); ++ p) {
    TpgNode* node = *p;
    Literal dlit(node->dvar(), false);
    odiff.push_back(dlit);
  }
  solver.add_clause(odiff);

  if ( mTimerEnable ) {
    mTimer.stop();
    mCnfTime += mTimer.time();
    ++ mCnfCount;
  }

  // 故障に対するテスト生成を行なう．
  mAssumptions.clear();
  mAssumptions.reserve(tfo_list.size());

  // dominator ノードの dvar は1でなければならない．
  for (TpgNode* node = f_tgt->node(); node != NULL; node = node->imm_dom()) {
    Literal dlit(node->dvar(), false);
    mAssumptions.push_back(dlit);
  }

  if ( f_tgt->is_input_fault() ) {
    fnode = fnode->fanin(f_tgt->pos());
  }
  bool inv = (f_tgt->val() != 0);
  mAssumptions.push_back(Literal(fnode->gvar(), inv));

  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }

  Bool3 ans = solver.solve(mAssumptions, mModel);
  if ( ans == kB3True ) {
    // パタンが求まった．

    // バックトレースを行う．
    TestVector* tv = bt(f_tgt->node(), mModel, mInputList, mOutputList);

    // パタンの登録などを行う．
    dop(f_tgt, tv);

    if ( mTimerEnable ) {
      mTimer.stop();
      mDetTime += mTimer.time();
      ++ mDetCount;
    }
  }
  else if ( ans == kB3False ) {
    // 検出不能と判定された．
    uop(f_tgt);

    if ( mTimerEnable ) {
      mTimer.stop();
      mUndetTime += mTimer.time();
      ++ mUndetCount;
    }
  }
  else { // ans == kB3X つまりアボート
    if ( mTimerEnable ) {
      mTimer.stop();
      mAbortTime += mTimer.time();
      ++ mAbortCount;
    }
  }

  clear_node_mark();

  update_stats(solver);
}

// @brief ノードの変数割り当てフラグを消す．
void
SatEngine2::clear_node_mark()
{
  for (vector<TpgNode*>::iterator p = mUsedNodeList.begin();
       p != mUsedNodeList.end(); ++ p) {
    TpgNode* node = *p;
    node->clear_var();
  }
  mUsedNodeList.clear();
}

// @brief 統計情報を得る．
// @param[in] stats 結果を格納する構造体
void
SatEngine2::get_stats(DtpgStats& stats) const
{
  stats.mCnfGenCount = mCnfCount;
  stats.mCnfGenTime = mCnfTime;
  stats.mDetCount = mDetCount;
  stats.mDetTime = mDetTime;
  stats.mRedCount = mUndetCount;
  stats.mRedTime = mUndetTime;
  stats.mAbortCount = mAbortCount;
  stats.mAbortTime = mAbortTime;

#if 0
  if ( mRunCount > 0 ) {
    cout << "# of runs:                       " << mRunCount << endl
	 << "# of problems:                   " << mSatCount << endl
	 << "Ave. # of restarts:              " << (double) mRestart / mSatCount << endl
	 << "Ave. # of variables:             " << (double) mVarNum / mRunCount << endl
	 << "Ave. # of constraint clauses:    " << (double) mConstrClauseNum / mRunCount << endl
	 << "Ave. # of constraint literals:   " << (double) mConstrLitNum / mRunCount << endl
	 << "Ave. # of learnt clauses:        " << (double) mLearntClauseNum / mRunCount << endl
	 << "Ave. # of learnt literals:       " << (double) mLearntLitNum / mRunCount << endl
	 << "Ave. # of conflicts:             " << (double) mConflictNum / mSatCount << endl
	 << "Ave. # of decisions:             " << (double) mDecisionNum / mSatCount << endl
	 << "Ave. # of implications:          " << (double) mPropagationNum / mSatCount << endl;
  }
#endif
}

// @breif 時間計測を制御する．
void
SatEngine2::timer_enable(bool enable)
{
  mTimerEnable = enable;
}

// @brief 統計情報を得る．
void
SatEngine2::update_stats(SatSolver& solver)
{
  SatStats sat_stat;
  solver.get_stats(sat_stat);

  ++ mRunCount;
  ++ mSatCount;
  mRestart += sat_stat.mRestart;
  mVarNum += sat_stat.mVarNum;
  mConstrClauseNum += sat_stat.mConstrClauseNum;
  mConstrLitNum += sat_stat.mConstrLitNum;
  mLearntClauseNum += sat_stat.mLearntClauseNum;
  mLearntLitNum += sat_stat.mLearntLitNum;
  mConflictNum += sat_stat.mConflictNum;
  mDecisionNum += sat_stat.mDecisionNum;
  mPropagationNum += sat_stat.mPropagationNum;
}

END_NAMESPACE_YM_SATPG