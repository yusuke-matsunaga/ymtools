﻿
/// @file BddMgrModern.cc
/// @brief BddMgrModern の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "BddMgrModern.h"
#include "BmmVar.h"
#include "BmmCompTbl.h"

#if !defined(__SUNPRO_CC) || __SUNPRO_CC >= 0x500
#include <exception>
#include <new>

BEGIN_NAMESPACE_YM_BDD

typedef bad_alloc alloc_xcpt;

#else
#include <exception.h>

BEGIN_NAMESPACE_YM_BDD

typedef xalloc alloc_xcpt;

#endif

#define BDDMGR_USE_MALLOC

BEGIN_NONAMESPACE

// 1K = 1,024
const ymuint64 K_unit = (1 << 10);
// 1M = 1,024 x 1,024
const ymuint64 M_unit = (1 << 20);

// パラメータのデフォルト値
const double DEFAULT_GC_THRESHOLD  = 0.10;
const ymuint64 DEFAULT_GC_NODE_LIMIT =  64 * K_unit;
const double DEFAULT_NT_LOAD_LIMIT = 2.0;
const double DEFAULT_RT_LOAD_LIMIT = 0.8;
const ymuint64 DEFAULT_MEM_LIMIT     = 400 * M_unit;
const ymuint64 DEFAULT_DZONE         =  10 * M_unit;

// 節点テーブルの初期サイズ
const ymuint64 INIT_SIZE = 1 * K_unit;

// 一度にアロケートするノード数
const ymuint64 NODE_UNIT = 1 * K_unit;

// 変数テーブルの初期サイズ
const ymuint64 VARTABLE_INIT_SIZE = 1 * K_unit;

// 節点テーブルのハッシュ関数
inline
ymuint64
hash_func2(BddEdge id1,
	   BddEdge id2)
{
  ymuint64 v1 = id1.hash();
  ymuint64 v2 = id2.hash();
  return v1 + (v2 >> 2);
}

// 節点テーブルのハッシュ関数
inline
ymuint64
hash_func3(BddEdge id1,
	   BddEdge id2,
	   ymuint level)
{
  ymuint64 v1 = id1.hash();
  ymuint64 v2 = id2.hash();
  ymuint64 id3 = level;
  return v1 + (v2 >> 2) + (id3 << 3) - id3;
}

// VarId 用のハッシュ関数
inline
ymuint64
var_hash(VarId var)
{
  ymuint64 key = var.val();
  return ((key * key) >> 8) + key;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// BddMgrModern
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] name 名前
// @param[in] option オプション
BddMgrModern::BddMgrModern(const string& name,
			   const string& option) :
  mName(name)
{
  if ( mName == string() ) {
    // 適当な名前を付ける．
    static int num = 1;
    ostringstream s;
    s << "bdd_mgr#" << num ++;
    mName = s.str();
  }

  bool reorder = (option == "reorder");

  // ユーザー設定可能パラメータのデフォルト値を設定
  mGcThreshold = DEFAULT_GC_THRESHOLD;
  mGcNodeLimit = DEFAULT_GC_NODE_LIMIT;
  mNtLoadLimit = DEFAULT_NT_LOAD_LIMIT;
  mRtLoadLimit = DEFAULT_RT_LOAD_LIMIT;
  mMemLimit = DEFAULT_MEM_LIMIT;
  mDangerousZone = DEFAULT_DZONE;
  mGcEnable = 0;

  // メモリ管理用のメンバを初期化
  mFreeTop = nullptr;
  mFreeNum = 0;
  mTopBlk = nullptr;
  mCurBlk = nullptr;
  mCurIdx = 1;

  // 内部情報の初期化
  mUsedMem = 0;
  mNodeNum = 0;
  mGarbageNum = 0;
  mGcCount = 0;

  // 節点テーブルの初期化
  mTableSize = 0;
  mTableSize_1 = 0;
  mNextLimit = 0;
  mNodeTable = nullptr;
  if ( !reorder ) {
    resize(INIT_SIZE);
  }

  // 変数テーブルの初期化
  mVarTableSize = VARTABLE_INIT_SIZE;
  mVarTable = alloc_vartable(mVarTableSize);
  ASSERT_COND(mVarTable );
  mVarHashTable = alloc_vartable(mVarTableSize);
  ASSERT_COND(mVarHashTable );
  mVarNum = 0;
  mMaxLevel = 0;

  // 演算結果テーブルの初期化
  mTblTop = nullptr;

  mCmpTable = new CompTbl1(this, "compose_table");
  ASSERT_COND(mCmpTable );

  mSmTable = new CompTbl1(this, "sm_table");
  ASSERT_COND(mSmTable );
  mAeTable = new CompTbl2(this, "ae_table");
  ASSERT_COND(mAeTable );
  mCubedivTable = new CompTbl1(this, "cubediv_table");
  ASSERT_COND(mCubedivTable );

  mMinsupTable = new CompTbl2(this, "minsup_table");
  ASSERT_COND(mMinsupTable );
  mIsopTable = new IsopTbl(this, "isop_table");
  ASSERT_COND(mIsopTable );
  mPcTable = new IsopTbl(this, "pc_table");
  ASSERT_COND(mPcTable );

  mCsTable = new CompTbl2(this, "cs_table");
  ASSERT_COND(mCsTable );
  mCs1Table = new CompTbl3(this, "cs1_table");
  ASSERT_COND(mCs1Table );
  mCs2Table = new CompTbl2(this, "cs2_table");
  ASSERT_COND(mCs2Table );
}

// デストラクタ
BddMgrModern::~BddMgrModern()
{
  // 節点テーブルの解放
  dealloc_nodetable(mNodeTable, mTableSize);

  // 節点用のメモリブロックの解放
  for (BddNode* blk = mTopBlk; blk; ) {
    BddNode* temp = blk;
    blk = temp->mLink;
    dealloc_nodechunk(temp);
  }
  if ( mCurBlk ) {
    dealloc_nodechunk(mCurBlk);
  }

  // 演算結果テーブルの解放
  for (CompTbl* tbl = mTblTop; tbl; ) {
    CompTbl* tmp = tbl;
    tbl = tbl->mNext;
    delete tmp;
  }

  // 変数の解放
  for (ymuint i = 0; i < mVarNum; ++ i) {
    delete mVarTable[i];
  }

  // 変数テーブルの解放
  dealloc_vartable(mVarTable, mVarTableSize);
  dealloc_vartable(mVarHashTable, mVarTableSize);

  // このマネージャに関わるメモリはすべて解放したはず．
  ASSERT_COND( mUsedMem == 0 );
}

// 変数を確保する．
// 確保に失敗したら false を返す．
// 最後の変数の後ろに挿入される．
bool
BddMgrModern::new_var(VarId varid)
{
  BmmVar* var = var_of(varid);
  if ( var == nullptr ) {
    return alloc_var(varid) != nullptr;
  }
  else {
    return true;
  }
}

// 変数を確保する．
// 最後の変数の後ろに挿入される．
BmmVar*
BddMgrModern::alloc_var(VarId varid)
{
  if ( mVarTableSize == mVarNum ) {
    ymuint64 new_size = mVarTableSize << 1;
    BmmVar** new_table = alloc_vartable(new_size);
    if ( !new_table ) {
      return nullptr;
    }
    BmmVar** new_hash = alloc_vartable(new_size);
    if ( !new_hash ) {
      dealloc_vartable(new_table, new_size);
      return nullptr;
    }
    for (ymuint i = 0; i < mVarNum; ++ i) {
      BmmVar* var = mVarTable[i];
      new_table[i] = var;
      reg_var(var);
    }
    dealloc_vartable(mVarTable, mVarTableSize);
    dealloc_vartable(mVarHashTable, mVarTableSize);
    mVarTable = new_table;
    mVarHashTable = new_hash;
    mVarTableSize = new_size;
  }
  ASSERT_COND(mVarTableSize > mVarNum );
  BmmVar* var = new BmmVar(this, varid);
  if ( var ) {
    reg_var(var);
    mVarTable[mVarNum] = var;
    var->mLevel = mVarNum;
    ++ mVarNum;
    if ( mMaxLevel < var->mLevel ) {
      mMaxLevel = var->mLevel;
    }
  }
  return var;
}

// 現在登録されている変数をそのレベルの昇順で返す．
ymuint
BddMgrModern::var_list(list<VarId>& vlist) const
{
  vlist.clear();
  for (ymuint i = 0; i < mVarNum; ++ i) {
    vlist.push_back(mVarTable[i]->varid());
  }
  return mVarNum;
}

// 変数番号からレベルを得る．
// もしもレベルが割り当てられていない場合にはエラーとなる．
ymuint
BddMgrModern::level(VarId varid) const
{
  BmmVar* v = var_of(varid);
  return v->level();
}

// レベルから変数番号を得る．
VarId
BddMgrModern::varid(ymuint level) const
{
  BmmVar* v = var_at(level);
  return v->varid();
}

// Var 側に節点テーブルを持つとき true を返す．
bool
BddMgrModern::is_reorderable() const
{
  return mNodeTable == nullptr;
}

// @brief 現在の最大レベル + 1を返す．
ymuint
BddMgrModern::max_level() const
{
  return mMaxLevel + 1;
}

// 動的変数順変更を許可する．
void
BddMgrModern::enable_DVO()
{
}

// 動的変数順変更を禁止する．
void
BddMgrModern::disable_DVO()
{
}

// 節点テーブルを次に拡大する時の基準値を計算する．
void
BddMgrModern::set_next_limit_size()
{
  // 制限値はロード値のリミットと現在のテーブルサイズから計算される
  if ( is_reorderable() ) {
    for (ymuint i = 0; i < mVarNum; ++ i) {
      mVarTable[i]->set_next_limit_size(mNtLoadLimit);
    }
  }
  else {
    mNextLimit = static_cast<ymuint64>(double(mTableSize) * mNtLoadLimit);
  }
}

// @brief ガーベージコレクションを許可する．
void
BddMgrModern::enable_gc()
{
  if ( mGcEnable > 0 ) {
    -- mGcEnable;
  }
}

// @brief ガーベージコレクションを禁止する．
void
BddMgrModern::disable_gc()
{
  ++ mGcEnable;
}

// ガーベージコレクションを行なう．
// 具体的には各ノードの参照回数が0のノードをフリーリストに回収し
// 再利用できるよ うにする．
// その際，それらのノードに関係した演算結果テーブルの内容はクリアされる．
// shrink_nodetable = true の時, 可能なら節点テーブルのサイズを縮小する．
void
BddMgrModern::_gc(bool shrink_nodetable)
{
  logstream() << "BddMgrModern::GC() begin...." << endl;

  // 演算結果テーブルをスキャンしておかなければならない．
  for (CompTbl* tbl = mTblTop; tbl; tbl = tbl->mNext) {
    if ( tbl->used_num() > 0 ) tbl->sweep();
  }

  // その他のテーブルの sweep を行う．
  mSweepMgr.prop_event();

  // 節点テーブルをスキャンしてゴミを取り除く
  if ( is_reorderable() ) {
    for (ymuint i = 0; i < mVarNum; ++ i) {
      mVarTable[i]->sweep();
    }
  }
  else {
    BddNode** ptr = mNodeTable;
    BddNode** end = mNodeTable + mTableSize;
    do {
      BddNode* temp;
      BddNode** prev = ptr;
      while ( (temp = *prev) ) {
	if ( temp->noref() ) {
	  // どこからも参照されていないノードは節点テーブルから除く
	  // この時点ではフリーリストを作る必要は無い．
	  *prev = temp->mLink;
	  temp->mLink = nullptr;
	}
	else {
	  prev = &(temp->mLink);
	}
      }
    } while ( ++ ptr != end );
  }

  // 確保したメモリブロック順にフリーリストを整列させる．
  // 整列といってもO(n)で実行できる．
  // メモリブロックの中身が全てフリーリストに含まれるのならば
  // そのメモリブロックを本当に解放する．
  // その時には，このブロックに含まれるノードはフリーリストから除かなければ
  // ならない．
  mFreeTop = nullptr;
  BddNode** prev = &mFreeTop;
  BddNode** prev_blk = &mTopBlk;
  int delete_num = 0;
  for (BddNode* blk; (blk = *prev_blk);  ) {
    if ( scan_nodechunk(blk, NODE_UNIT, prev) ) {
      *prev_blk = blk->mLink;
      dealloc_nodechunk(blk);
      delete_num += NODE_UNIT - 1;
    }
    else {
      prev_blk = &(blk->mLink);
    }
  }

  // カレントブロックを調べる．
  if ( mCurBlk ) {
    if ( scan_nodechunk(mCurBlk, mCurIdx, prev) ) {
      dealloc_nodechunk(mCurBlk);
      delete_num += mCurIdx - 1;
      mCurBlk = nullptr;
    }
  }
  *prev_blk = nullptr;
  *prev = nullptr;

  // 内部 statistics の更新
  ymuint64 free_num = mGarbageNum;
  mFreeNum += free_num - delete_num;
  mNodeNum -= free_num;
  mGarbageNum = 0;
  ++ mGcCount;

  if ( shrink_nodetable ) {
    // ノードテーブルが縮小可能ならば縮小する
    if ( is_reorderable() ) {
      for (ymuint i = 0; i < mVarNum; ++ i) {
	mVarTable[i]->shrink(mNtLoadLimit);
      }
    }
    else {
      ymuint64 nn = static_cast<ymuint64>(mNodeNum * 2.0 / mNtLoadLimit);
      ymuint64 new_size = mTableSize;
      while ( new_size > INIT_SIZE ) {
	if ( nn < new_size ) {
	  new_size >>= 1;
	}
	else {
	  break;
	}
      }
      if ( new_size != mTableSize ) {
	resize(new_size);
      }
    }
  }

  logstream() << "BddMgrModern::GC() end." << endl
	      << "  " << free_num
	      << " nodes are deleted from the node-table." << endl
	      << "  " << delete_num << " nodes are (really) freed."
	      << endl;
}

// 節点テーブルを拡張する
// メモリアロケーションに失敗したら false を返す．
bool
BddMgrModern::resize(ymuint64 new_size)
{
  logstream() << "BddMgrModern::resize(" << new_size << ")" << endl;

  BddNode** new_table = alloc_nodetable(new_size);
  if ( !new_table ) {
    // アロケーションに失敗した．
    return false;
  }

  ymuint64 old_size = mTableSize;
  BddNode** old_table = mNodeTable;
  mNodeTable = new_table;
  mTableSize = new_size;
  mTableSize_1 = mTableSize - 1;
  set_next_limit_size();
  BddNode** tbl = old_table;
  if ( tbl ) {
    BddNode** end = tbl + old_size;
    do {
      BddNode* next;
      BddNode* temp;
      for (temp = *tbl; temp; temp = next) {
	next = temp->mLink;
	ymuint64 pos = hash_func3(temp->edge0(), temp->edge1(), temp->level());
	BddNode*& entry = mNodeTable[pos & mTableSize_1];
	temp->mLink = entry;
	entry = temp;
      }
    } while ( ++ tbl != end );
    dealloc_nodetable(old_table, old_size);
  }
  return true;
}

// GC 前の sweep 処理を行うためのバインダーを登録する．
void
BddMgrModern::reg_sweep_binder(EventBinder* binder)
{
  mSweepMgr.reg_binder(binder);
}

// さまざまなパラメータを設定する．
// 設定方法はX-windowのライブラリ関数に良くあるやり方で，まず，全ての
// パラメータを格納する構造体bdd_param_tに必要なパラメータを設定し，
// 設定したいパラメータに対応するビットのみをmaskにセットする．
void
BddMgrModern::param(const BddMgrParam& param,
		    ymuint32 mask)
{
  if ( mask & BddMgrParam::GC_THRESHOLD ) {
    mGcThreshold = param.mGcThreshold;
  }
  if ( mask & BddMgrParam::GC_NODE_LIMIT ) {
    mGcNodeLimit = param.mGcNodeLimit;
  }
  if ( mask & BddMgrParam::NT_LOAD_LIMIT ) {
    mNtLoadLimit = param.mNtLoadLimit;
    set_next_limit_size();
  }
  if ( mask & BddMgrParam::RT_LOAD_LIMIT ) {
    mRtLoadLimit = param.mRtLoadLimit;
    for (CompTbl* tbl = mTblTop; tbl; tbl = tbl->mNext) {
      tbl->load_limit(mRtLoadLimit);
    }
  }
  if ( mask & BddMgrParam::MEM_LIMIT ) {
    mMemLimit = param.mMemLimit;
  }
}

// パラメータを読み出す
void
BddMgrModern::param(BddMgrParam& param) const
{
  param.mGcThreshold = mGcThreshold;
  param.mGcNodeLimit = mGcNodeLimit;
  param.mNtLoadLimit = mNtLoadLimit;
  param.mRtLoadLimit = mRtLoadLimit;
  param.mMemLimit = mMemLimit;
}

// 名前を得る．
const string&
BddMgrModern::name() const
{
  return mName;
}

// さまざまな内部情報を得る
// これらの変数はread-onlyなので，変数自体を外部で宣言せずに，
// それを読み出す関数を定義している．
ymuint64
BddMgrModern::used_mem() const
{
  return mUsedMem;
}

ymuint64
BddMgrModern::node_num() const
{
  return mNodeNum;
}

ymuint64
BddMgrModern::garbage_num() const
{
  return mGarbageNum;
}

ymuint64
BddMgrModern::avail_num() const
{
  return mFreeNum;
}

ymuint64
BddMgrModern::gc_count() const
{
  return mGcCount;
}

// 同一の節点が存在するか調べ，ない時にのみ新たなノードを確保する
// 使用メモリ量が上限を越えたら kEdgeInvalid を返す．
BddEdge
BddMgrModern::new_node(ymuint level,
		       BddEdge e0,
		       BddEdge e1)
{
  if ( e0 == e1 ) {
    return e0;
  }

  if ( e0.is_error() || e1.is_error() ) {
    return BddEdge::make_error();
  }
  if ( e0.is_overflow() || e1.is_overflow() ) {
    return BddEdge::make_overflow();
  }

  // 否定枝に関する正規化ルール
  bool ans_inv = e0.inv();
  e0.add_inv(ans_inv);
  e1.add_inv(ans_inv);

  BddNode* temp;
  ymuint64 pos;
  BmmVar* var = nullptr;
  // 節点テーブルを探す．
  if ( is_reorderable() ) {
    var = var_at(level);
    pos = hash_func2(e0, e1);
    for (temp = var->mNodeTable[pos & var->mTableSize_1]; temp;
	 temp = temp->mLink) {
      if ( temp->edge0() == e0 && temp->edge1() == e1 ) {
	// 同一の節点がすでに登録されている
	goto already_exist;
      }
    }
  }
  else {
    pos = hash_func3(e0, e1, level);
    for (temp = mNodeTable[pos & mTableSize_1]; temp; temp = temp->mLink) {
      if ( temp->edge0() == e0 &&
	   temp->edge1() == e1 &&
	   temp->level() == level ) {
	// 同一の節点がすでに登録されている
	goto already_exist;
      }
    }
  }

  // 節点テーブルには登録されていなかったので新しい節点を取ってきて
  // 内容を設定する．
  temp = alloc_node();
  if ( !temp ) {
    // メモリアロケーションに失敗した
    return BddEdge::make_overflow();
  }
  temp->mEdge0 = e0;
  temp->mEdge1 = e1;
  temp->mLevel = level;
  temp->mRefMark = 0UL;  // mark = none, link = 0

  // 新たなノードを登録する．
  if ( is_reorderable() ) {
    var->reg_node(pos, temp);
  }
  else {
    if ( mNodeNum > mNextLimit ) {
      // ノード数が制限値を越えたのでテーブルを2倍に拡張する
      resize(mTableSize << 1);
    }
    BddNode*& entry = mNodeTable[pos & mTableSize_1];
    temp->mLink = entry;
    entry = temp;
  }

 already_exist:

  BddEdge ans(temp, ans_inv);

  return ans;
}

// e の参照回数を増やす．
void
BddMgrModern::inc_rootref(BddEdge e)
{
  activate(e);
}

// e の参照回数を減らす．
void
BddMgrModern::dec_rootref(BddEdge e)
{
  deactivate(e);

  // ゴミがたまっていたら回収する．
  // ただし頻繁に gc() を呼びたくないので条件をもうけている．
  if ( mGcEnable == 0 &&
       mNodeNum > mGcNodeLimit	&&
       mGarbageNum > static_cast<ymuint64>(double(mNodeNum) * mGcThreshold) ) {
    gc(false);
  }
}

// mVarTable 中のマークを消す．
void
BddMgrModern::clear_varmark()
{
  for (ymuint i = 0; i < mVarNum; ++ i) {
    mVarTable[i]->mMark = 0;
  }
  mVarSet.clear();
}

// level の変数を取り出す．
BmmVar*
BddMgrModern::var_at(ymuint level) const
{
  ASSERT_COND(level < mVarNum );
  return mVarTable[level];
}

// varid の変数を取出す．
BmmVar*
BddMgrModern::var_of(VarId varid) const
{
  ymuint64 pos = var_hash(varid) & (mVarTableSize - 1);
  for (BmmVar* var = mVarHashTable[pos]; var; var = var->mLink) {
    if ( var->varid() == varid ) {
      return var;
    }
  }
  // 見つからなかった．
  return nullptr;
}

// Var を登録する．
void
BddMgrModern::reg_var(BmmVar* var)
{
  ymuint64 pos = var_hash(var->varid()) & (mVarTableSize - 1);
  BmmVar*& entry = mVarHashTable[pos];
  var->mLink = entry;
  entry = var;
}

// @brief lockall() 用のフック
void
BddMgrModern::lock_hook(BddNode* vp)
{
  -- mGarbageNum;
}

// @brief unlockall() 用のフック
void
BddMgrModern::unlock_hook(BddNode* vp)
{
  ++ mGarbageNum;
}

// 演算結果テーブルを登録する．
void
BddMgrModern::add_table(CompTbl* tbl)
{
  tbl->mNext = mTblTop;
  mTblTop = tbl;
}

// 節点を確保する．
BddNode*
BddMgrModern::alloc_node()
{
  BddNode* temp;

  if ( (temp = mFreeTop) ) {
    // フリーリストに節点があったのでそれを取り出す．
    mFreeTop = temp->mLink;
    -- mFreeNum;
  }
  else {
    // ないので新たに確保する．
    if ( !mCurBlk ) {
      // 使用可能なメモリブロックもない．
      mCurBlk = alloc_nodechunk();
      if ( !mCurBlk ) {
	// メモリアロケーションに失敗した
	return nullptr;
      }
      mCurIdx = 1;
    }

    temp = &mCurBlk[mCurIdx];
    ++ mCurIdx;
    if ( mCurIdx == NODE_UNIT ) {
      // ブロック内のノードを使い切った．
      // メモリブロックのリストへ繋げる．
      mCurBlk->mLink = mTopBlk;
      mTopBlk = mCurBlk;
      mCurBlk = nullptr;
    }
  }
  ++ mGarbageNum;  // この時点では誰も指していない．
  ++ mNodeNum;

  return temp;
}

// 節点チャンクをスキャンして参照されていない節点をフリーリストにつなぐ
// ただし，チャンク全体が参照されていなかった場合にはフリーリストには
// つながない．その場合には true を返す．
bool
BddMgrModern::scan_nodechunk(BddNode* blk,
			     ymuint64 blk_size,
			     BddNode**& prev)
{
  BddNode** prev_orig = prev;
  bool can_delete = true;
  // メモリブロックの先頭のノードは次のブロックを指すポインタとして
  // 使っているのでスキャンから除外する．
  BddNode* temp = &blk[1];
  for (ymuint64 i = 1; i < blk_size; ++ i, ++ temp) {
    if ( temp->noref() ) {
      *prev = temp;
      prev = &(temp->mLink);
    }
    else {
      can_delete = false;
    }
  }
  if ( can_delete ) {
    prev = prev_orig;
  }
  return can_delete;
}

// 変数テーブル用のメモリを確保する．
// size はバイト単位ではなくエントリ数．
BmmVar**
BddMgrModern::alloc_vartable(ymuint64 size)
{
  ymuint64 byte_size = sizeof(BmmVar*) * size;
  void* ptr = allocate(byte_size);
  if ( ptr ) {
    memset(ptr, 0, byte_size);
  }
  return static_cast<BmmVar**>(ptr);
}

// 変数テーブル用のメモリを解放する．
// size はバイト単位ではなくエントリ数
void
BddMgrModern::dealloc_vartable(BmmVar** table,
			       ymuint64 size)
{
  ymuint64 byte_size = sizeof(BmmVar*) * size;
  deallocate(table, byte_size);
}

// 節点テーブル用のメモリを確保する．
// size はバイト単位ではなくエントリ数
BddNode**
BddMgrModern::alloc_nodetable(ymuint64 size)
{
  ymuint64 byte_size = sizeof(BddNode*) * size;
  void* ptr = allocate(byte_size);
  if ( ptr ) {
    memset(ptr, 0, byte_size);
  }
  return static_cast<BddNode**>(ptr);
}

// 節点テーブル用のメモリを解放する．
// size はバイト単位ではなくエントリ数
void
BddMgrModern::dealloc_nodetable(BddNode** table,
				ymuint64 size)
{
  ymuint64 byte_size = sizeof(BddNode*) * size;
  deallocate(table, byte_size);
}

// 節点チャンク用のメモリを確保する．
BddNode*
BddMgrModern::alloc_nodechunk()
{
  const ymuint64 byte_size = sizeof(BddNode) * NODE_UNIT;
  void* ptr = allocate(byte_size);
  return static_cast<BddNode*>(ptr);
}

// 節点チャンク用のメモリを解放する．
void
BddMgrModern::dealloc_nodechunk(BddNode* chunk)
{
  const ymuint64 byte_size = sizeof(BddNode) * NODE_UNIT;
  deallocate(chunk, byte_size);
}

// BDD パッケージ用のメモリ確保ルーティン
void*
BddMgrModern::allocate(ymuint64 size)
{
  if ( mUsedMem + size > mMemLimit ) {
    // メモリ制限をオーバーしたので 0 を返す．
    return 0;
  }
  mUsedMem += size;
  void* ans;

#ifdef BDDMGR_USE_MALLOC
  ans = malloc(size);
  if ( !ans ) {
    return 0;
  }
#else
  try {
    ans = new char[size];
  }
  catch (alloc_xcpt x) {
    // new が失敗した．
    return 0;
  }
#endif

#if defined(BDD_DEBUG_MEMALLOC)
  {
    ios::flags save = logstream().flags();
    logstream() << "BddMgrModern::allocate(" << size << ") --> "
		<< setw(8) << hex << (unsigned int) ans << endl;
    logstream().flags(save);
  }
#endif
  return ans;
}

// BDD パッケージ用のメモリ解放ルーティン
void
BddMgrModern::deallocate(void* ptr,
			 ymuint64 size)
{
#if defined(BDD_DEBUG_MEMALLOC)
  {
    ios::flags save = logstream().flags();
    logstream() << "BddMgrModern::deallocate(" << setw(8) << hex
		<< (unsigned int) ptr << ", " << size << ")" << endl;
    logstream().flags(save);
  }
#endif

  mUsedMem -= size;

#ifdef BDDMGR_USE_MALLOC
  free(ptr);
#else
  memset(ptr, 0xa5, size);
  delete [] (char*)ptr;
#endif
}

END_NAMESPACE_YM_BDD
