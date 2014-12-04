#ifndef ASTBLOCK_H
#define ASTBLOCK_H

/// @file AstBlock.h
/// @brief AstBlock のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "ymsl_int.h"
#include "YmUtils/HashMap.h"
#include "YmUtils/ShString.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
/// @class AstBlock AstBlock.h "AstBlock.h"
/// @brief ブロックを表すクラス
//////////////////////////////////////////////////////////////////////
class AstBlock
{
public:

  /// @brief コンストラクタ
  /// @param[in] parent 親のブロック
  AstBlock(AstBlock* parent);

  /// @brief デストラクタ
  ~AstBlock();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief statement を追加する．
  void
  add_statement(AstStatement* statement);

  /// @brief 関数定義を追加する．
  /// @param[in] item 追加する要素
  void
  add_funcdecl(AstFuncDecl* item);

  /// @brief 変数定義を追加する．
  /// @param[in] item 追加する要素
  void
  add_vardecl(AstVarDecl* item);

  /// @brief 名前から要素を見つける．
  /// @param[in] name 名前
  /// @return name という名前の要素を返す．
  ///
  /// ここになければ親のブロックを探す．
  /// それでもなければ NULL を返す．
  YmslAst*
  find_item(ShString name) const;

  /// @brief 内容を表示する．(デバッグ用)
  /// @param[in] s 出力ストリーム
  /// @param[in] indent インデントレベル
  void
  print(ostream& s,
	ymuint indent = 0) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 親のブロック
  AstBlock* mParent;

  // シンボルテーブル
  HashMap<ShString, YmslAst*> mSymbolTable;

  // statement リスト
  vector<AstStatement*> mStatementList;

};

END_NAMESPACE_YM_YMSL

#endif // ASTBLOCK_H