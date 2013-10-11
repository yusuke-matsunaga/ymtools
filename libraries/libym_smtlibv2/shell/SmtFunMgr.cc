
/// @file SmtFunMgr.cc
/// @brief SmtFunMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#include "SmtFunMgr.h"
#include "SmtFunImpl.h"
#include "ym_smtlibv2/SmtId.h"


BEGIN_NAMESPACE_YM_SMTLIBV2

//////////////////////////////////////////////////////////////////////
// クラス SmtFunMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] alloc メモリアロケータ
// @param[in] level スタックレベル
// @param[in] parent_mgr 上位のマネージャ
SmtFunMgr::SmtFunMgr(Alloc& alloc,
		     ymuint level,
		     SmtFunMgr* parent_mgr) :
  mAlloc(alloc),
  mLevel(level),
  mParent(parent_mgr)
{
  mNum = 0;
  mTableSize = 0;
  mHashTable = NULL;

  expand_table(1024);
}

// @brief デストラクタ
SmtFunMgr::~SmtFunMgr()
{
  delete [] mHashTable;
}

// @brief 宣言のみの関数を返す．
// @param[in] name_id 名前を表す識別子
// @param[in] sort 出力の型
// @param[in] input_list 入力の型のリスト
// @param[in] attr 属性
// @param[in] param_num パラメータの数
const SmtFun*
SmtFunMgr::reg_fun(const SmtId* name_id,
		   const vector<const SmtSort*>& input_list,
		   const SmtSort* sort,
		   SmtFun::tAttr attr,
		   ymuint param_num)
{
  if ( find_fun(name_id) != NULL ) {
    // 同名の関数が登録されている．
    return NULL;
  }

  if ( mNum >= mNextLimit ) {
    expand_table(mTableSize * 2);
  }

  ymuint n = input_list.size();
  void* p = mAlloc.get_memory(sizeof(SmtFun1) + sizeof(const SmtSort*) * (n - 1));
  SmtFun1* fun = new (p) SmtFun1(name_id, sort, n, attr, param_num);

  ++ mNum;

  for (ymuint i = 0; i < n; ++ i) {
    fun->mInputList[i] = input_list[i];
  }

  ymuint h = name_id->id() % mTableSize;
  fun->mLink = mHashTable[h];
  mHashTable[h] = fun;

  return fun;
}

// @brief 宣言のみの関数を返す．
// @param[in] name 名前を表す識別子
// @param[in] sort 出力の型
// @param[in] input_list 入力の型と変数のリスト
// @param[in] body 本体
// @note input_list と input_var_list のサイズは同じ
const SmtFun*
SmtFunMgr::reg_fun(const SmtId* name_id,
		   const vector<SmtSortedVar>& input_list,
		   const SmtSort* sort,
		   const SmtTerm* body)
{
  if ( find_fun(name_id) != NULL ) {
    // 同名の関数が登録されている．
    return NULL;
  }

  if ( mNum >= mNextLimit ) {
    expand_table(mTableSize * 2);
  }

  ymuint n = input_list.size();
  void* p = mAlloc.get_memory(sizeof(SmtFun2) + sizeof(SmtSortedVar) * (n - 1));
  SmtFun2* fun = new (p) SmtFun2(name_id, sort, n, body);

  ++ mNum;

  for (ymuint i = 0; i < n; ++ i) {
    fun->mInputList[i] = input_list[i];
  }

  ymuint h = name_id->id() % mTableSize;
  fun->mLink = mHashTable[h];
  mHashTable[h] = fun;

  return fun;
}

// @brief 関数を返す．
// @param[in] name 名前
const SmtFun*
SmtFunMgr::find_fun(const SmtId* name_id) const
{
  if ( mParent != NULL ) {
    // 親のレベルで探す．
    const SmtFun* fun = mParent->find_fun(name_id);
    if ( fun != NULL ) {
      return fun;
    }
  }

  ymuint h = name_id->id();
  ymuint idx = h % mTableSize;
  for (SmtFunImpl* fun = mHashTable[idx];
       fun != NULL; fun = fun->mLink) {
    if ( fun->name() == name_id ) {
      return fun;
    }
  }
  return NULL;
}

// @brief ハッシュ表を拡大する．
// @param[in] req_size 新しいサイズ
void
SmtFunMgr::expand_table(ymuint req_size)
{
  ymuint old_size = mTableSize;
  SmtFunImpl** old_table = mHashTable;

  mTableSize = req_size;
  mHashTable = new SmtFunImpl*[mTableSize];
  for (ymuint i = 0; i < mTableSize; ++ i) {
    mHashTable[i] = NULL;
  }
  mNextLimit = static_cast<ymuint32>(mTableSize * 1.8);

  if ( old_size > 0 ) {
    for (ymuint i = 0; i < old_size; ++ i) {
      for (SmtFunImpl* fun = old_table[i]; fun != NULL; ) {
	SmtFunImpl* tmp_fun = fun;
	fun = fun->mLink;

	ymuint h = tmp_fun->name()->id() % mTableSize;
	tmp_fun->mLink = mHashTable[h];
	mHashTable[h] = tmp_fun;
      }
    }
    delete [] old_table;
  }
}

END_NAMESPACE_YM_SMTLIBV2
