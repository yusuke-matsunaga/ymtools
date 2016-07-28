
/// @file BddMgrRef.cc
/// @brief BddMgrRef の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_logic/BddMgr.h"
#include "ym_logic/BddVector.h"
#include "ym_logic/BddList.h"

#include "BddMgrImpl.h"

#include "ym_utils/HeapTree.h"


BEGIN_NAMESPACE_YM_BDD

BddMgr BddMgr::theDefaultMgr;

BEGIN_NONAMESPACE

// 入力数がこの値よりも小さかったらヒープ木は使わない．
const ymuint kNiLimit = 8;

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス BddMgr
//////////////////////////////////////////////////////////////////////

// @brief デフォルトマネージャを返す．
BddMgr&
BddMgr::default_mgr()
{
  return theDefaultMgr;
}

// @brief デフォルトマネージャを作るためのコンストラクタ
BddMgr::BddMgr() :
  mImpl(BddMgrImpl::default_mgr())
{
}

// @brief コンストラクタ
// @param[in] type BddMgr の型を表す文字列
// @param[in] name マネージャの名前
// @param[in] option オプション文字列
BddMgr::BddMgr(const string& type,
	       const string& name,
	       const string& option)
{
  mImpl = BddMgrImpl::new_mgr(type, name, option);
  assert_cond(mImpl, __FILE__, __LINE__);
}

// デストラクタ
BddMgr::~BddMgr()
{
  if ( mImpl != BddMgrImpl::default_mgr() ) {
    // デフォルトマネージャは削除しない．
    delete mImpl;
  }
}

// 定数0を表すBDDを作る．
Bdd
BddMgr::make_zero()
{
  return Bdd(mImpl, BddEdge::make_zero());
}

// 定数1を表すBDDを作る．
Bdd
BddMgr::make_one()
{
  return Bdd(mImpl, BddEdge::make_one());
}

// エラーを表すBDDを作る．
Bdd
BddMgr::make_error()
{
  return Bdd(mImpl, BddEdge::make_error());
}

// オーバーフローを表すBDDを作る．
Bdd
BddMgr::make_overflow()
{
  return Bdd(mImpl, BddEdge::make_overflow());
}

// リテラル関数を表すBDDを作る．
Bdd
BddMgr::make_literal(VarId index,
		     tPol pol)
{
  return Bdd(mImpl, mImpl->make_literal(index, pol));
}

// リテラル関数を表すBDDを作る
Bdd
BddMgr::make_literal(const Literal& lit)
{
  return make_literal(lit.varid(), lit.pol());
}

// 肯定のリテラル関数を作る
Bdd
BddMgr::make_posiliteral(VarId varid)
{
  return Bdd(mImpl, mImpl->make_posiliteral(varid));
}

// 否定のリテラル関数を作る．
Bdd
BddMgr::make_negaliteral(VarId varid)
{
  return Bdd(mImpl, mImpl->make_negaliteral(varid));
}

// 指定されたインデックスを持つ中間ノードを一つ持つBDDを作る
// その左右の子のデフォルトはそれぞれ0と1
// メモリ不足によってエラーが起きる可能性がある．
Bdd
BddMgr::make_bdd(VarId index,
		 const Bdd& chd0,
		 const Bdd& chd1)
{
  Bdd idx_bdd = make_posiliteral(index);
  return ite_op(idx_bdd, chd1, chd0);
}

// ベクタを真理値表と見なしてBDDを作る．
// 変数は 0 〜 (ni - 1) とみなす．
// ベクタの値は非ゼロを true とみなす．
// v の大きさは 2^ni に等しくなければならない．
Bdd
BddMgr::tvec_to_bdd(const vector<int>& v,
		    ymuint ni)
{
  VarVector vv(ni);
  for (ymuint i = 0; i < ni; ++ i) {
    vv[i] = VarId(i);
  }
  return tvec_to_bdd(v, vv);
}

// ベクタを真理値表と見なしてBDDを作る．
// こちらは個々の変数を vars で指定したもの．
// ベクタの値は非ゼロを true とみなす．
// v の大きさは 2^(vars.size()) に等しくなければならない．
Bdd
BddMgr::tvec_to_bdd(const vector<int>& v,
		    const VarVector& vars)
{
  return Bdd(mImpl, mImpl->tvec_to_bdd(v, vars));
#if DEBUG_MAKE_BDD
  ymuint ni = vars.size();
  ymuint nv = (1 << ni);
  Bdd ans0(ans, mImpl);
  BddVector varbdds(ni);
  for (ymuint i = 0; i < ni; ++ i) {
    VarId vid = vars[i];
    varbdds[i] = make_posiliteral(vid);
  }

  Bdd ans1 = make_zero();
  for (ymuint p = 0; p < nv; ++ p) {
    if ( v[p] ) {
      Bdd minterm = make_one();
      for (ymuint i = 0; i < ni; ++ i) {
	if ( p & (1 << (ni - i - 1)) ) {
	  minterm &= varbdds[i];
	}
	else {
	  minterm &= ~varbdds[i];
	}
      }
      ans1 |= minterm;
    }
  }
  assert_cond(ans0 == ans1, __FILE__, __LINE__);
  return ans0;
#endif
}

// Expr から対応するBDDを作り出す．
// 論理式中に現れるリテラルを置き換えるBDDを varmap に入れる．
// varmap に登録されていないリテラルはそのまま使う．
Bdd
BddMgr::expr_to_bdd(const LogExpr& expr,
		    const VarBddMap& varmap)
{
  // 定数0の場合
  if ( expr.is_zero() ) {
    return make_zero();
  }

  // 定数1の場合
  if ( expr.is_one() ) {
    return make_one();
  }

  // リテラルの場合
  if ( expr.is_literal() ) {
    VarId pos = expr.varid();
    Bdd ans;
    if ( !varmap.find(pos, ans) ) {
      ans = make_posiliteral(pos);
    }
    if ( expr.is_negaliteral() ) {
      ans.negate();
    }
    return ans;
  }

  // ここまで来たら根はAND/OR/XOR演算子
  ymuint n = expr.child_num();
  BddVector bdd_list(*this, n);
  for (ymuint i = 0; i < n; ++ i) {
    Bdd bdd = expr_to_bdd(expr.child(i), varmap);
    bdd_list[i] = bdd;
  }
  if ( expr.is_and() ) {
    return bdd_list.and_op();
  }
  if ( expr.is_or() ) {
    return bdd_list.or_op();
  }
  if ( expr.is_xor() ) {
    return bdd_list.xor_op();
  }
  assert_not_reached(__FILE__, __LINE__);
  return make_error();
}

// LogExpr から対応するBDDを作り出す．
// 論理式中に現れるリテラルを置き換えるリテラル番号を varmap に入れる．
// varmap に登録されていないリテラルはそのまま使う．
Bdd
BddMgr::expr_to_bdd(const LogExpr& expr,
		    const VarVarMap& varmap)
{
  VarBddMap vbmap;
  for (HashMapIterator<VarId, VarId> p = varmap.begin();
       p != varmap.end(); ++ p) {
    VarId id = p.key();
    VarId id2 = p.value();
    Bdd bdd = make_posiliteral(id2);
    vbmap.add(id, bdd);
  }
  return expr_to_bdd(expr, vbmap);
}

// ストリームを読んで論理式を作り,そこからBDDを作る．
// エラーが起きたら err_msg をセットしてエラーBDDを返す．
Bdd
BddMgr::expr_to_bdd(istream& in,
		    string& err_msg)
{
  LogExpr expr = LogExpr::stream_to_expr(in, err_msg);
  if ( err_msg == string() ) {
    return expr_to_bdd(expr);
  }
  return make_error();
}

// 文字列を読んで論理式を作り,そこからBDDを作る．
// エラーが起きたら err_msg をセットしてエラーBDDを返す．
Bdd
BddMgr::expr_to_bdd(const char* s,
		    string& err_msg)
{
  istringstream in(s);
  return expr_to_bdd(in, err_msg);
}

// 文字列を読んで論理式を作り,そこからBDDを作る．
// エラーが起きたら err_msg をセットしてエラーBDDを返す．
Bdd
BddMgr::expr_to_bdd(const string& s,
		    string& err_msg)
{
  istringstream in(s.c_str());
  return expr_to_bdd(in, err_msg);
}

BEGIN_NONAMESPACE
// make_thfunc() 中で使われるサブルーティン
inline
ymuint
elem(ymuint i,
     ymuint j,
     ymuint th)
{
  return i * (th + 1) + j;
}
END_NONAMESPACE

// 閾値関数を作る．
Bdd
BddMgr::make_thfunc(ymuint n,
		    ymuint th)
{
  vector<BddEdge> table((n + 1) * (th + 1));
  table[elem(n, 0, th)] = BddEdge::make_one();
  for (ymuint i = 1; i <= th; i ++) {
    table[elem(n, i, th)] = BddEdge::make_zero();
  }
  for (ymuint i = n; i -- > 0; ) {
    BddEdge idx = mImpl->make_posiliteral(VarId(i));
    table[elem(i, 0, th)] = BddEdge::make_one();
    for (ymuint j = 1; j <= th; j ++) {
      if ( j > n - i ) {
	table[elem(i, j, th)] = BddEdge::make_zero();
      }
      else {
	BddEdge l = table[elem(i + 1, j, th)];
	BddEdge h = table[elem(i + 1, j - 1, th)];
	BddEdge tmp = mImpl->ite_op(idx, h, l);
	if ( tmp.is_overflow() ) {
	  return make_overflow();
	}
	table[elem(i, j, th)] = tmp;
      }
    }
  }
  BddEdge ans = table[elem(0, th, th)];
  return Bdd(mImpl, ans);
}

// 変数を確保する．
// 確保に失敗したら false を返す．
// 最後の変数の後ろに挿入される．
bool
BddMgr::new_var(VarId varid)
{
  return mImpl->new_var(varid);
}

// 現在登録されている変数をそのレベルの昇順で返す．
ymuint
BddMgr::var_list(list<VarId>& vlist) const
{
  return mImpl->var_list(vlist);
}

// 変数番号からレベルを得る．
// もしもレベルが割り当てられていない場合にはエラーとなる．
ymuint
BddMgr::level(VarId varid) const
{
  return mImpl->level(varid);
}

// レベルから変数番号を得る．
VarId
BddMgr::varid(ymuint level) const
{
  return mImpl->varid(level);
}

// 動的変数順変更を許可する．
void
BddMgr::enable_DVO()
{
  mImpl->enable_DVO();
}

// 動的変数順変更を禁止する．
void
BddMgr::disable_DVO()
{
  mImpl->disable_DVO();
}

// @brief ガーベージコレクションを許可する．
void
BddMgr::enable_gc()
{
  mImpl->enable_gc();
}

// @brief ガーベージコレクションを禁止する．
void
BddMgr::disable_gc()
{
  mImpl->disable_gc();
}

// ガーベージコレクションを行なう．
// 具体的には各ノードの参照回数が0のノードをフリーリストに回収し
// 再利用できるよ うにする．
// その際，それらのノードに関係した演算結果テーブルの内容はクリアされる．
// shrink_nodetable = true の時, 可能なら節点テーブルのサイズを縮小する．
void
BddMgr::gc(bool shrink_nodetable)
{
  mImpl->gc(shrink_nodetable);
}

// GC 前の sweep 処理を行うためのバインダーを登録する．
void
BddMgr::reg_sweep_binder(EventBinder* binder)
{
  mImpl->reg_sweep_binder(binder);
}

// ログ出力用のストリームを設定する．
void
BddMgr::set_logstream(ostream& s)
{
  mImpl->set_logstream(s);
}

// ログ出力用のストリームを解除する．
void
BddMgr::unset_logstream()
{
  mImpl->unset_logstream();
}

// ログ出力用のストリームを得る．
ostream&
BddMgr::logstream() const
{
  return mImpl->logstream();
}

// パラメータを設定する．設定したい項目のマスクビットを1とする．
void
BddMgr::param(const BddMgrParam& param,
	      ymuint32 mask)
{
  mImpl->param(param, mask);
}

// パラメータを取得する．
void
BddMgr::param(BddMgrParam& param) const
{
  mImpl->param(param);
}

// 名前を得る．
const string&
BddMgr::name() const
{
  return mImpl->name();
}

// 使用メモリ量(in bytes)を得る．
ymuint64
BddMgr::used_mem() const
{
  return mImpl->used_mem();
}

// 節点テーブルに登録されているノードの数を得る．
ymuint64
BddMgr::node_num() const
{
  return mImpl->node_num();
}

// GC で回収される(フリーになる)ノード数を得る．
ymuint64
BddMgr::garbage_num() const
{
  return mImpl->garbage_num();
}

// 利用可能なフリーノード数を得る．
ymuint64
BddMgr::avail_num() const
{
  return mImpl->avail_num();
}

// GC の起動された回数を得る．
ymuint64
BddMgr::gc_count() const
{
  return mImpl->gc_count();
}

END_NAMESPACE_YM_BDD
