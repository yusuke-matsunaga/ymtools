
/// @file libym_ymsh/YmshCmd.cc
/// @brief YmshCmd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_ymsh/YmshCmd.h"


BEGIN_NAMESPACE_YM_YMSH

//////////////////////////////////////////////////////////////////////
// クラス YmshCmd
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] name コマンド名
// @note name の領域は他のクラスで管理されているものとする．
YmshCmd::YmshCmd(const char* name) :
  mName(name),
  mYmsh(NULL)
{
}

// @brief デストラクタ
YmshCmd::~YmshCmd()
{
}

// @brief コマンド名を返す．
const char*
YmshCmd::name() const
{
  return mName.c_str();
}

// @brief 親のインタープリタを返す．
Ymsh*
YmshCmd::ymsh() const
{
  reutrn mYmsh;
}

END_NAMESPACE_YM_YMSH
