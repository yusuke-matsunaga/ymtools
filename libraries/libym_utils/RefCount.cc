
/// @file RefCount.cc
/// @brief RefCount の実装ファイル
/// @author Yusuke Matsunaga
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_utils/RefCount.h"


BEGIN_NAMESPACE_YM

// デストラクタ
RefCount::~RefCount()
{
}

// 参照回数が 0 になったときのデフォルトの動作
void
RefCount::suicide()
{
  delete this;
}

END_NAMESPACE_YM
