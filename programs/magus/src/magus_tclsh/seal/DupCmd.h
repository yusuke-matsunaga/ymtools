#ifndef SRC_SEAL_DUPCMD_H
#define SRC_SEAL_DUPCMD_H

/// @file src/seal/DupCmd.h
/// @brief DupCmd のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: DupCmd.h 2274 2009-06-10 07:45:29Z matsunaga $
///
/// Copyright (C) 2005-2010 Yusuke Matsunaga
/// All rights reserved.


#include "MagBNetCmd.h"
#include "seal.h"


BEGIN_NAMESPACE_MAGUS_SEAL

//////////////////////////////////////////////////////////////////////
// 回路を部分的に多重化するコマンド
//////////////////////////////////////////////////////////////////////
class DupCmd :
  public MagBNetCmd
{
public:

  /// @brief コンストラクタ
  DupCmd(NetMgr* mgr);

  /// @brief デストラクタ
  virtual
  ~DupCmd();


protected:
  
  // コマンド処理関数
  virtual
  int
  cmd_proc(TclObjVector& objv);

};

END_NAMESPACE_MAGUS_SEAL

#endif // SRC_SEAL_DUPCMD_H
