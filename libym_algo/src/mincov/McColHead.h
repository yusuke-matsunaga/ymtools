﻿#ifndef MCCOLHEAD_H
#define MCCOLHEAD_H

/// @file McColHead.h
/// @brief McMatrix のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "mincov_nsdef.h"
#include "McCell.h"
#include "YmUtils/UnitAlloc.h"


BEGIN_NAMESPACE_YM_MINCOV

//////////////////////////////////////////////////////////////////////
/// @class McColHead McColHead.h "McColHead.h"
/// @brief Mc 用の列の先頭要素を表すクラス
//////////////////////////////////////////////////////////////////////
class McColHead
{
  friend class McMatrix;

private:

  /// @brief コンストラクタ
  /// @param[in] pos 列番号
  McColHead(ymuint pos);

  /// @brief デストラクタ
  ~McColHead();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 列番号を返す．
  ymuint32
  pos() const;

  /// @brief 要素数を返す．
  ymuint32
  num() const;

  /// @brief 先頭の要素を返す．
  const McCell*
  front() const;

  /// @brief 末尾の要素を返す．
  const McCell*
  back() const;

  /// @brief cell が終端かどうか調べる．
  /// @param[in] cell 対象の要素
  /// @return cell が終端の時 true を返す．
  bool
  is_end(const McCell* cell) const;

  /// @brief 直前の列を返す．
  const McColHead*
  prev() const;

  /// @brief 直後の列を返す．
  const McColHead*
  next() const;


public:

  mutable
  ymuint32 mWork;

private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続している要素をクリアする．
  void
  clear();

  /// @brief 追加する要素の両脇を求める．
  /// @param[in] cell 対象の要素
  /// @retval true 正しく処理された．
  /// @retval false 同一の要素が存在した．
  /// @note cell の mDownLink, mUpLink に結果がセットされる．
  bool
  search_insert_pos(McCell* cell);

  /// @brief 要素を追加する．
  /// @param[in] cell 追加する要素
  /// @note cell の mDownLink, mUpLink に適切な値がセットされているものとする．
  void
  insert_elem(McCell* cell);

  /// @brief 要素を削除する．
  /// @param[in] cell 削除する要素
  void
  delete_elem(McCell* cell);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 列番号
  ymuint32 mPos;

  // 要素数
  ymuint32 mNum;

  // 二重連結リストのダミーヘッダ
  McCell mDummy;

  // 直前の列を指すリンク
  McColHead* mPrev;

  // 直後の列を指すリンク
  McColHead* mNext;

  // 削除フラグ
  bool mDeleted;
};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] pos 列番号
inline
McColHead::McColHead(ymuint pos) :
  mPos(pos),
  mNum(0),
  mPrev(nullptr),
  mNext(nullptr),
  mDeleted(false)
{
  mDummy.mUpLink = &mDummy;
  mDummy.mDownLink = &mDummy;
}

/// @brief デストラクタ
inline
McColHead::~McColHead()
{
}

// @brief 列番号を返す．
inline
ymuint32
McColHead::pos() const
{
  return mPos;
}

// @brief 要素数を返す．
inline
ymuint32
McColHead::num() const
{
  return mNum;
}

// @brief 先頭の要素を返す．
inline
const McCell*
McColHead::front() const
{
  return mDummy.mDownLink;
}

// @brief 末尾の要素を返す．
inline
const McCell*
McColHead::back() const
{
  return mDummy.mUpLink;
}

// @brief cell が終端かどうか調べる．
// @param[in] cell 対象の要素
// @return cell が終端の時 true を返す．
inline
bool
McColHead::is_end(const McCell* cell) const
{
  return cell == &mDummy;
}

// @brief 直前の列を返す．
inline
const McColHead*
McColHead::prev() const
{
  return mPrev;
}

// @brief 直後の列を返す．
inline
const McColHead*
McColHead::next() const
{
  return mNext;
}

// @brief 接続している要素をクリアする．
inline
void
McColHead::clear()
{
  mDummy.mUpLink = &mDummy;
  mDummy.mDownLink = &mDummy;
  mNum = 0;
}

// @brief 要素を追加する．
// @param[in] cell 追加する要素
// @note cell の mDownLink, mUpLink に適切な値がセットされているものとする．
inline
void
McColHead::insert_elem(McCell* cell)
{
  McCell* prev = cell->mUpLink;
  McCell* next = cell->mDownLink;

  if ( prev->mDownLink != cell ) {
    ++ mNum;
  }

  prev->mDownLink = cell;
  next->mUpLink = cell;
}

// @brief 要素を削除する．
// @param[in] cell 削除する要素
inline
void
McColHead::delete_elem(McCell* cell)
{
  McCell* prev = cell->mUpLink;
  McCell* next = cell->mDownLink;

  prev->mDownLink = next;
  next->mUpLink = prev;
  -- mNum;
}

END_NAMESPACE_YM_MINCOV

#endif // MCCOLHEAD_H
