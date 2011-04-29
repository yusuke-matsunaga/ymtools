#ifndef LIBYM_CELL_DOTLIB_PTMGR_H
#define LIBYM_CELL_DOTLIB_PTMGR_H

/// @file libym_cell/dotlib/PtMgr.h
/// @brief PtMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "dotlib_nsdef.h"
#include "ym_utils/ShString.h"
#include "ym_utils/FileRegion.h"
#include "ym_utils/Alloc.h"


BEGIN_NAMESPACE_YM_CELL_DOTLIB

//////////////////////////////////////////////////////////////////////
/// @class PtMgr PtMgr.h "PtMgr.h"
/// @brief PtNode を管理するためのクラス
//////////////////////////////////////////////////////////////////////
class PtMgr
{
public:

  /// @brief コンストラクタ
  PtMgr();

  /// @brief デストラクタ
  ~PtMgr();


public:

  /// @brief 初期化する．
  /// @note 以前，生成されたオブジェクトは(デストラクタ抜きで)破壊される．
  void
  init();

  /// @brief 整数値を表す PtNode を生成する．
  /// @param[in] value 値
  /// @param[in] loc ファイル上の位置
  PtNode*
  new_int(int value,
	  const FileRegion& loc);

  /// @brief 実数値を表す PtNode を生成する．
  /// @param[in] value 値
  /// @param[in] loc ファイル上の位置
  PtNode*
  new_float(double value,
	    const FileRegion& loc);

  /// @brief 文字列シンボルを表す PtNode を生成する．
  /// @param[in] value 値
  /// @param[in] loc ファイル上の位置
  PtNode*
  new_string(ShString value,
	     const FileRegion& loc);

  /// @brief + 演算子を表す PtNode を生成する．
  /// @param[in] opr1, opr2 オペランド
  PtNode*
  new_plus(PtNode* opr1,
	   PtNode* opr2);

  /// @brief - 演算子を表す PtNode を生成する．
  /// @param[in] opr1, opr2 オペランド
  PtNode*
  new_minus(PtNode* opr1,
	    PtNode* opr2);

  /// @brief * 演算子を表す PtNode を生成する．
  /// @param[in] opr1, opr2 オペランド
  PtNode*
  new_mult(PtNode* opr1,
	   PtNode* opr2);

  /// @brief / 演算子を表す PtNode を生成する．
  /// @param[in] opr1, opr2 オペランド
  PtNode*
  new_div(PtNode* opr1,
	  PtNode* opr2);

  /// @brief リストを表す PtNode を生成する．
  /// @note 空のリストが返される．
  PtNode*
  new_list();

  /// @brief グループを表す PtNode を生成する．
  /// @param[in] value 値
  /// @param[in] loc ファイル上の位置
  PtNode*
  new_group(const PtNode* value,
	    const FileRegion& loc);

  /// @brief PtAttr を生成する．
  PtAttr*
  new_attr(const ShString& attr_name,
	   const PtNode* value);

  /// @brief 使用メモリ量の一覧を出力する．
  /// @param[in] s 出力先のストリーム
  void
  show_stats(ostream& s);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // メモリアロケータ
  SimpleAlloc mAlloc;

  // 個々の要素の使用数
  ymuint32 mIntNum;
  ymuint32 mFloatNum;
  ymuint32 mStrNum;
  ymuint32 mOprNum;
  ymuint32 mListNum;
  ymuint32 mGroupNum;
  ymuint32 mAttrNum;
};

END_NAMESPACE_YM_CELL_DOTLIB

#endif // LIBYM_CELL_DOTLIB_PTMGR_H