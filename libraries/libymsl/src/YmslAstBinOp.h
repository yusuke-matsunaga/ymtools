#ifndef YMSLASTBINOP_H
#define YMSLASTBINOP_H

/// @file YmslAstBinOp.h
/// @brief YmslAstBinOp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmslAstImpl.h"


BEGIN_NAMESPACE_YM_YMSL

//////////////////////////////////////////////////////////////////////
/// @ckass YmslAstBinOp YmslAstBinOp.h "YmslAstBinOp.h"
/// @brief 二項演算子を表す YmslAst
//////////////////////////////////////////////////////////////////////
class YmslAstBinOp :
  public YmslAstImpl
{
public:

  /// @breif コンストラクタ
  /// @param[in] left, right オペランド
  YmslAstBinOp(YmslAst* left,
	       YmslAst* right);

  /// @brief デストラクタ
  virtual
  ~YmslAstBinOp();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 子供の数を返す．
  virtual
  ymuint
  child_num() const;

  /// @brief 子供を返す．
  /// @param[in] pos 位置( 0 <= pos < child_num() )
  virtual
  YmslAst*
  child(ymuint pos) const;

  /// @brief 内容を表示する．(デバッグ用)
  /// @param[in] s 出力ストリーム
  virtual
  void
  print(ostream& s) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 子供のノード
  YmslAst* mChild[2];

};


//////////////////////////////////////////////////////////////////////
/// @class YmslAstPlus YmslAstBinOp.h "YmslAstBinOp.h"
/// @brief 加算を表す YmslAst
//////////////////////////////////////////////////////////////////////
class YmslAstPlus :
  public YmslAstBinOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] left, right オペランド
  YmslAstPlus(YmslAst* left,
	      YmslAst* right);

  /// @brief デストラクタ
  virtual
  ~YmslAstPlus();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を得る．
  virtual
  AstType
  type() const;

};


//////////////////////////////////////////////////////////////////////
/// @class YmslAstMinus YmslAstBinOp.h "YmslAstBinOp.h"
/// @brief 減算を表す YmslAst
//////////////////////////////////////////////////////////////////////
class YmslAstMinus :
  public YmslAstBinOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] left, right オペランド
  YmslAstMinus(YmslAst* left,
	       YmslAst* right);

  /// @brief デストラクタ
  virtual
  ~YmslAstMinus();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を得る．
  virtual
  AstType
  type() const;

};


//////////////////////////////////////////////////////////////////////
/// @class YmslAstMult YmslAstBinOp.h "YmslAstBinOp.h"
/// @brief 乗算を表す YmslAst
//////////////////////////////////////////////////////////////////////
class YmslAstMult :
  public YmslAstBinOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] left, right オペランド
  YmslAstMult(YmslAst* left,
	      YmslAst* right);

  /// @brief デストラクタ
  virtual
  ~YmslAstMult();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を得る．
  virtual
  AstType
  type() const;

};


//////////////////////////////////////////////////////////////////////
/// @class YmslAstDiv YmslAstBinOp.h "YmslAstBinOp.h"
/// @brief 除算を表す YmslAst
//////////////////////////////////////////////////////////////////////
class YmslAstDiv :
  public YmslAstBinOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] left, right オペランド
  YmslAstDiv(YmslAst* left,
	     YmslAst* right);

  /// @brief デストラクタ
  virtual
  ~YmslAstDiv();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を得る．
  virtual
  AstType
  type() const;

};


//////////////////////////////////////////////////////////////////////
/// @class YmslAstMod YmslAstBinOp.h "YmslAstBinOp.h"
/// @brief 剰余を表す YmslAst
//////////////////////////////////////////////////////////////////////
class YmslAstMod :
  public YmslAstBinOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] left, right オペランド
  YmslAstMod(YmslAst* left,
	     YmslAst* right);

  /// @brief デストラクタ
  virtual
  ~YmslAstMod();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を得る．
  virtual
  AstType
  type() const;

};


//////////////////////////////////////////////////////////////////////
/// @class YmslAstBitAnd YmslAstBinOp.h "YmslAstBinOp.h"
/// @brief ビットANDを表す YmslAst
//////////////////////////////////////////////////////////////////////
class YmslAstBitAnd :
  public YmslAstBinOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] left, right オペランド
  YmslAstBitAnd(YmslAst* left,
		YmslAst* right);

  /// @brief デストラクタ
  virtual
  ~YmslAstBitAnd();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を得る．
  virtual
  AstType
  type() const;

};


//////////////////////////////////////////////////////////////////////
/// @class YmslAstBitOr YmslAstBinOp.h "YmslAstBinOp.h"
/// @brief ビットORを表す YmslAst
//////////////////////////////////////////////////////////////////////
class YmslAstBitOr :
  public YmslAstBinOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] left, right オペランド
  YmslAstBitOr(YmslAst* left,
	       YmslAst* right);

  /// @brief デストラクタ
  virtual
  ~YmslAstBitOr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を得る．
  virtual
  AstType
  type() const;

};


//////////////////////////////////////////////////////////////////////
/// @class YmslAstBitXor YmslAstBinOp.h "YmslAstBinOp.h"
/// @brief ビットXORを表す YmslAst
//////////////////////////////////////////////////////////////////////
class YmslAstBitXor :
  public YmslAstBinOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] left, right オペランド
  YmslAstBitXor(YmslAst* left,
		YmslAst* right);

  /// @brief デストラクタ
  virtual
  ~YmslAstBitXor();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を得る．
  virtual
  AstType
  type() const;

};


//////////////////////////////////////////////////////////////////////
/// @class YmslAstLogAnd YmslAstBinOp.h "YmslAstBinOp.h"
/// @brief 論理積を表す YmslAst
//////////////////////////////////////////////////////////////////////
class YmslAstLogAnd :
  public YmslAstBinOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] left, right オペランド
  YmslAstLogAnd(YmslAst* left,
		YmslAst* right);

  /// @brief デストラクタ
  virtual
  ~YmslAstLogAnd();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を得る．
  virtual
  AstType
  type() const;

};


//////////////////////////////////////////////////////////////////////
/// @class YmslAstLogOr YmslAstBinOp.h "YmslAstBinOp.h"
/// @brief 論理和を表す YmslAst
//////////////////////////////////////////////////////////////////////
class YmslAstLogOr :
  public YmslAstBinOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] left, right オペランド
  YmslAstLogOr(YmslAst* left,
	       YmslAst* right);

  /// @brief デストラクタ
  virtual
  ~YmslAstLogOr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を得る．
  virtual
  AstType
  type() const;

};


//////////////////////////////////////////////////////////////////////
/// @class YmslAstLogXor YmslAstBinOp.h "YmslAstBinOp.h"
/// @brief 排他的論理和を表す YmslAst
//////////////////////////////////////////////////////////////////////
class YmslAstLogXor :
  public YmslAstBinOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] left, right オペランド
  YmslAstLogXor(YmslAst* left,
		YmslAst* right);

  /// @brief デストラクタ
  virtual
  ~YmslAstLogXor();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を得る．
  virtual
  AstType
  type() const;

};

END_NAMESPACE_YM_YMSL

#endif // YMSLASTBINOP_H
