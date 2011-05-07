
/// @file libym_ymsh/Ymsh.cc
/// @brief Ymsh の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_ymsh/Ymsh.h"
#include "YmshImpl.h"

#include "SetCmd.h"
#include "AliasCmd.h"


BEGIN_NAMESPACE_YM_YMSH

//////////////////////////////////////////////////////////////////////
// クラス YmshBase
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
YmshBase::YmshBase() :
  mImpl(new YmshImpl)
{
  YmshCmd* set_cmd = new SetCmd;
  mImpl->reg_command(set_cmd);

  YmshCmd* unset_cmd = new UnsetCmd;
  mImpl->reg_command(unset_cmd);

  YmshCmd* alias_cmd = new AliasCmd;
  mImpl->reg_command(alias_cmd);

  YmshCmd* unalias_cmd = new UnaliasCmd;
  mImpl->reg_command(unalias_cmd);
}

// @brief デストラクタ
YmshBase::~YmshBase()
{
  delete mImpl;
}

// @brief コマンドループを実行する．
void
YmshBase::run()
{
  mImpl->run();
}


//////////////////////////////////////////////////////////////////////
// クラス Ymsh
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Ymsh::Ymsh()
{
}

// @brief デストラクタ
Ymsh::~Ymsh()
{
}

END_NAMESPACE_YM_YMSH
