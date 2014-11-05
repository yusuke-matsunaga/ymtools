
/// @file LR0Set.cc
/// @brief LR0Set の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "LR0Set.h"
#include "Grammer.h"
#include "LR0State.h"
#include "LR0Term.h"
#include "Rule.h"
#include "Token.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// LR0Term の比較関数
struct LR0TermLt
{
  bool
  operator()(const LR0Term& left,
	     const LR0Term& right)
  {
    ymuint l_id = left.rule()->id();
    ymuint r_id = right.rule()->id();
    if ( l_id < r_id ) {
      return true;
    }
    if ( l_id > r_id ) {
      return false;
    }
    // l_id == r_id

    ymuint l_pos = left.dot_pos();
    ymuint r_pos = right.dot_pos();
    if ( l_pos < r_pos ) {
      return true;
    }
    return false;
  }
};

// @brief 項集合の閉包を求める．
// @param[in] input 入力の項集合
// @param[out] output 閉包集合
void
closure(const vector<LR0Term>& input,
	vector<LR0Term>& output)
{
  // 入力をコピーする．
  ymuint n = input.size();
  for (ymuint i = 0; i < n; ++ i) {
    output.push_back(input[i]);
  }

  // output は次に処理すべき項を入れるキューとしても機能している．
  // rpos が次に処理する項の位置を示す．
  // これが末尾に達したら新たに加わった項がないということ．
  for (ymuint rpos = 0; rpos < output.size(); ++ rpos) {
    const LR0Term& term = output[rpos];
    const Token* next_token = term.next_token();
    if ( next_token != NULL ) {
      // dot の次のトークンを左辺に持つ規則を列挙する．
      const vector<const Rule*>& rule_list = next_token->rule_list();
      for (vector<const Rule*>::const_iterator p = rule_list.begin();
	   p != rule_list.end(); ++ p) {
	const Rule* rule1 = *p;
	// rule1 を output に加える．
	// ただし，重複していたらなにもしない．
	// TODO: 線形探索でいいのか実際の例でサイズを調査する．
	bool found = false;
	for (ymuint i = 0; i < output.size(); ++ i) {
	  if ( output[i] == LR0Term(rule1, 0) ) {
	    found = true;
	    break;
	  }
	}
	if ( !found ) {
	  output.push_back(LR0Term(rule1, 0));
	}
      }
    }
  }

  // ソートしておく．
  sort(output.begin(), output.end(), LR0TermLt());
}

// @brief 遷移先の項集合を求める．
// @param[in] cur_state 現在の状態
// @param[in] token 次のトークン
// @param[out] next_terms 遷移先の状態を表す項集合
void
next_state(LR0State* cur_state,
	   const Token* token,
	   vector<LR0Term> next_terms)
{
  // 次のトークンが token に等しい項の dot を進めた項を tmp_terms に入れる．
  vector<LR0Term> tmp_terms;
  const vector<LR0Term>& cur_terms = cur_state->term_list();
  for (vector<LR0Term>::const_iterator p = cur_terms.begin();
       p != cur_terms.end(); ++ p) {
    const LR0Term& term = *p;
    if ( term.next_token() == token ) {
      const Rule* rule = term.rule();
      ymuint pos = term.dot_pos();
      tmp_terms.push_back(LR0Term(rule, pos + 1));
    }
  }
  // その閉包を求める．
  closure(tmp_terms, next_terms);
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス LR0Set
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] grammer 元となる文法
LR0Set::LR0Set(Grammer* grammer)
{
  // 初期状態は明示的に作る．
  // start_state = closure( {S'-> . S} )
  vector<LR0Term> start_terms;
  const vector<const Rule*>& rule_list = grammer->token(0)->rule_list();
  ASSERT_COND ( rule_list.size() == 1 );
  start_terms.push_back(LR0Term(rule_list[0], 0));
  vector<LR0Term> tmp_terms;
  closure(start_terms, tmp_terms);
  mStartState = new_state(tmp_terms);

  // mStateList に未処理の状態が残っている限り以下の処理を繰り返す．
  for (ymuint rpos = 0; rpos < mStateList.size(); ++ rpos) {
    LR0State* cur_state = mStateList[rpos];
    // cur_state に関係するトークンを取り出す．
    const vector<const Token*>& token_list = cur_state->token_list();
    for (vector<const Token*>::const_iterator p = token_list.begin();
	 p != token_list.end(); ++ p) {
      const Token* token = *p;
      // token に対する次状態を求める．
      vector<LR0Term> tmp_terms;
      next_state(cur_state, token, tmp_terms);
      // tmp_terms に対応する状態を作る．
      // 場合によっては既存の状態を再利用する．
      LR0State* state1 = new_state(tmp_terms);
      // それを cur_state の遷移先に設定する．
      cur_state->add_next_state(token, state1);
    }
  }
}

// @brief デストラクタ
LR0Set::~LR0Set()
{
  for (vector<LR0State*>::iterator p = mStateList.begin();
       p != mStateList.end(); ++ p) {
    delete *p;
  }
}

// @brief 状態のリストを返す．
const vector<LR0State*>&
LR0Set::state_list() const
{
  return mStateList;
}

// @brief 初期状態を返す．
LR0State*
LR0Set::start_state() const
{
  return mStartState;
}

// @brief 状態を追加する．
// @param[in] terms 状態を表す項集合
// @return 対応する状態を返す．
//
// すでに等価は状態が存在したらその状態を返す．
LR0State*
LR0Set::new_state(const vector<LR0Term>& terms)
{
  // たぶん効率のすごく悪い実装法
  for (ymuint i = 0; i < mStateList.size(); ++ i) {
    LR0State* state1 = mStateList[i];
    if ( state1->term_list() == terms ) {
      // すでに同じ状態が存在した．
      return state1;
    }
  }
  // なかったので新たに作る．
  ymuint id = mStateList.size();
  LR0State* state = new LR0State(id, terms);
  mStateList.push_back(state);
  return state;
}

END_NAMESPACE_YM
