﻿
/// @file StrPool.cc
/// @brief StrPool と ShString の実装ファイル
/// @author Yusuke Matsunaga
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmUtils/ShString.h"
#include "StrPool.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
// 文字列を共有するためのプール
//////////////////////////////////////////////////////////////////////

// ハッシュ関数
inline
ymuint32
StrPool::hash_func(const char* str)
{
  ymuint32 h = 0;
  ymuint32 c;
  for ( ; (c = static_cast<ymuint32>(*str)); ++ str) {
    h = h * 37 + c;
  }
  return h;
}

// コンストラクタ
StrPool::StrPool() :
  mCellAlloc(4096)
{
  alloc_table(1024);
  mNum = 0;
}

// デストラクタ
StrPool::~StrPool()
{
  delete [] mTable;
}

// 文字列を登録する．
const char*
StrPool::reg(const char* str)
{
  if ( mTableSize == 0 ) {
    alloc_table(1024);
  }

  // まず str と同一の文字列が登録されていないか調べる．
  ymuint32 hash_value = hash_func(str);
  ymuint32 pos = hash_value & mHashMask;
  for (Cell* cell = mTable[pos]; cell; cell = cell->mLink) {
    if ( memcmp(str, cell->mStr, cell->mSize) == 0 ) {
      return cell->mStr;
    }
  }

  // なければ新しい Cell を用意する．
  // とその前にテーブルの拡張が必要かどうか調べる．
  if ( mNum >= mExpandLimit ) {
    Cell** old_table = mTable;
    ymuint32 old_size = mTableSize;
    alloc_table(mTableSize << 1);
    for (ymuint32 i = 0; i < old_size; ++ i) {
      Cell* next;
      for (Cell* cell = old_table[i]; cell; cell = next) {
	next = cell->mLink;
	ymuint32 pos = hash_func(cell->mStr) & mHashMask;
	cell->mLink = mTable[pos];
	mTable[pos] = cell;
      }
    }
    delete [] old_table;
    // サイズを拡張したので pos が古くなっている．
    pos = hash_value & mHashMask;
  }

  ymuint len = static_cast<ymuint>(strlen(str));
  ymuint cell_size = len + sizeof(Cell);
  void* p = mCellAlloc.get_memory(cell_size);
  Cell* new_cell = new (p) Cell;
  memcpy(new_cell->mStr, str, len + 1);
  new_cell->mSize = len + 1;
  new_cell->mLink = mTable[pos];
  mTable[pos] = new_cell;
  ++ mNum;
  return new_cell->mStr;
}

// 確保した文字列領域の総量を得る．
ymuint64
StrPool::accum_alloc_size() const
{
  return mCellAlloc.allocated_size();
}

// @brief メモリを全部開放する．
// @note 非常に破壊的なのでメモリリーク検査時の終了直前などの場合のみに使う．
void
StrPool::destroy()
{
  delete [] mTable;
  mCellAlloc.destroy();
  mTableSize = 0;
  mTable = nullptr;
}

// テーブルを確保して初期化する．
void
StrPool::alloc_table(ymuint32 new_size)
{
  mTableSize = new_size;
  mHashMask = mTableSize - 1;
  mExpandLimit = static_cast<ymuint32>(mTableSize * 1.8);
  mTable = new Cell*[mTableSize];
  for (ymuint32 i = 0; i < mTableSize; ++ i) {
    mTable[i] = nullptr;
  }
}


//////////////////////////////////////////////////////////////////////
// ShString
//////////////////////////////////////////////////////////////////////

BEGIN_NONAMESPACE

StrPool thePool;

END_NONAMESPACE

// 共有文字列を作ってセットする．
void
ShString::set(const char* str)
{
  mPtr = thePool.reg(str);
}

// @brief ShString 関連でアロケートされたメモリサイズ
ymuint64
ShString::allocated_size()
{
  return thePool.accum_alloc_size();
}

// @brief ShString 関連でアロケートされたメモリをすべて開放する．
// @note 非常に破壊的なのでメモリリーク検査時の終了直前などの場合のみに使う．
void
ShString::free_all_memory()
{
  thePool.destroy();
}

// ShString 用ストリーム出力演算子
ostream&
operator<<(ostream& s,
	   const ShString& str)
{
  // ちょっと見にくいけど ShString から const char* への変換演算子
  const char* p = str.operator const char*();
  if ( p ) {
    return s << p;
  }
  else {
    return s << "";
  }
}

END_NAMESPACE_YM
