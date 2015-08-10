﻿#ifndef DECLMAP_H
#define DECLMAP_H

/// @file DeclMap.h
/// @brief DeclMap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmNetworks/mvn.h"
#include "YmVerilog/vl/VlFwd.h"
#include "YmUtils/UnitAlloc.h"


BEGIN_NAMESPACE_YM_NETWORKS_MVN_VERILOG

//////////////////////////////////////////////////////////////////////
/// @class DeclMap DeclMap.h "DeclMap.h"
/// @brief VlDecl と MvnNode の対応付けを行うハッシュ表
//////////////////////////////////////////////////////////////////////
class DeclMap
{
public:

  /// @brief コンストラクタ
  DeclMap();

  /// @brief デストラクタ
  ~DeclMap();


public:

  /// @brief 内部のデータをクリアする．
  void
  clear();

  /// @brief 登録する(単一要素の場合)
  /// @param[in] decl 宣言要素
  /// @param[in] node 対応するノード
  void
  add(const VlDecl* decl,
      MvnNode* node);

  /// @brief 登録する(配列の場合)
  /// @param[in] decl 宣言要素
  /// @param[in] offset
  /// @param[in] node 対応するノード
  void
  add(const VlDecl* decl,
      ymuint offset,
      MvnNode* node);

  /// @brief 対応するノードを取り出す．
  /// @param[in] decl 宣言要素
  /// @return 対応するノードを返す．
  /// @note 登録されていない場合と配列型の場合には nullptr を返す．
  MvnNode*
  get(const VlDecl* decl) const;

  /// @brief 対応するノードを取り出す(配列型)．
  /// @param[in] decl 宣言要素
  /// @param[in] offset オフセット
  /// @return 対応するノードを返す．
  /// @note 登録されていない場合と配列型でない場合，
  /// オフセットが範囲外の場合には nullptr を返す．
  MvnNode*
  get(const VlDecl* decl,
      ymuint offset) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  struct Cell
  {
    // 宣言要素
    const VlDecl* mDecl;

    // オフセット
    ymuint32 mOffset;

    // 対応するノード
    MvnNode* mNode;

    // 次の要素を指すリンク
    Cell* mLink;

  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief Cell を登録する．
  void
  put_cell(const VlDecl* decl,
	   ymuint offset,
	   MvnNode* node);

  /// @brief Cell を探す．
  Cell*
  find_cell(const VlDecl* decl,
	    ymuint offset) const;

  /// @brief テーブルの領域を確保する．
  /// @param[in] size 必要なサイズ
  void
  alloc_table(ymuint size);

  /// @brief ハッシュ値を計算する．
  ymuint
  hash_func(const VlDecl* decl,
	    ymuint offset) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // Cell の確保用アロケータ
  UnitAlloc mAlloc;

  // ハッシュ表のサイズ
  ymuint32 mSize;

  // ハッシュ表
  Cell** mTable;

  // ハッシュ表を拡大するしきい値
  ymuint32 mLimit;

  // 要素数
  ymuint32 mNum;

};

END_NAMESPACE_YM_NETWORKS_MVN_VERILOG

#endif // DECLMAP_H
