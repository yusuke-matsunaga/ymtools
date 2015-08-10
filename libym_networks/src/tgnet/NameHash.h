﻿#ifndef NAMEHASH_H
#define NAMEHASH_H

/// @file NameHash.h
/// @brief NameHash のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmNetworks/tgnet.h"
#include "YmUtils/SimpleAlloc.h"


BEGIN_NAMESPACE_YM_NETWORKS_TGNET

//////////////////////////////////////////////////////////////////////
/// @class NameHash NameHash.h "NameHash.h"
/// @brief 名前をキーにして TgNode を格納するハッシュ表
//////////////////////////////////////////////////////////////////////
class NameHash
{
public:

  /// @brief コンストラクタ
  NameHash();

  /// @brief デストラクタ
  ~NameHash();


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をクリアする．
  void
  clear();

  /// @brief 名前からノードを検索する
  /// @param[in] name 検索対象の名前
  /// @return name という名前のノードを返す．なければ nullptr を返す．
  TgNode*
  find_node(const char* name);

  /// @brief ノードを登録する．
  /// @param[in] name 登録対象の名前
  /// @param[in] node 登録対象のノード
  /// @return node にセットすべき文字列ポインタ( name ではない)を返す．
  const char*
  reg_node(const char* name,
	   TgNode* node);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  // ハッシュ表を拡大する．
  void
  alloc_table(size_t new_size);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いる構造体
  //////////////////////////////////////////////////////////////////////

  // 名前をキーにしてノードを求めるためのハッシュ用のセル
  struct StrIdCell
  {

    // コンストラクタ
    StrIdCell(TgNode* node,
	      const char* name);

    // ノード
    TgNode* mNode;
    // 次のセルを指すリンク
    StrIdCell* mLink;
    // 文字列領域を指すダミー
    char mStr[1];
  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ハッシュ用のアロケータ
  SimpleAlloc mAlloc;

  // 名前をキーにして ID を納めたハッシュ表
  StrIdCell** mHashTable;

  // ハッシュ表のサイズ
  size_t mTableSize;

  // ハッシュに登録されている要素数
  size_t mNum;

  // ハッシュ表を拡張する目安
  size_t mNextLimit;

};

END_NAMESPACE_YM_NETWORKS_TGNET

#endif // NAMEHASH_H
