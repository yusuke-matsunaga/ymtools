#ifndef LIBYM_VERILOG_ELABORATOR_INCLUDE_ELBDELAY_H
#define LIBYM_VERILOG_ELABORATOR_INCLUDE_ELBDELAY_H

/// @file libym_verilog/elaborator/include/ElbDelay.h
/// @brief ElbDelay のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_verilog/vl/VlDelay.h"


BEGIN_NAMESPACE_YM_VERILOG

//////////////////////////////////////////////////////////////////////
/// @class ElbDelay ElbDelay.h "ym_verilog/vl/VlDelay.h"
/// @brief エラボレーション中の expression を表す基底クラス
//////////////////////////////////////////////////////////////////////
class ElbDelay :
  public VlDelay
{
protected:

  /// @brief コンストラクタ
  ElbDelay() { }

  /// @brief デストラクタ
  virtual
  ~ElbDelay() { }

};

END_NAMESPACE_YM_VERILOG

#endif // LIBYM_VERILOG_ELABORATOR_INCLUDE_ELBDELAY_H