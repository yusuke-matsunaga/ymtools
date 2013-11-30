
/// @file Literal.cc
/// @brief Literal の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: Literal.cc 750 2007-06-30 09:09:58Z matsunaga $
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_logic/Literal.h"
#include "ym_logic/VarId.h"


BEGIN_NAMESPACE_YM

// 実はデフォルトコンストラクタが未定義リテラルを作っている．
const Literal kLiteralX;

// 場所がないのでここに書いておく．
const VarId kVarIdIllegal;

// ostream に対する書出し
ostream&
operator<<(ostream& s,
	   const Literal& lit)
{
  if ( lit == kLiteralX ) {
    s << "-X-";
  }
  else {
    s << "v_" << lit.varid();
    if ( lit.pol() == kPolNega ) {
      s << "'";
    }
  }
  return s;
}

END_NAMESPACE_YM
