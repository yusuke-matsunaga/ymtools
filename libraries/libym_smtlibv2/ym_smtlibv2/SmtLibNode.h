#ifndef YM_SMTLIBV2_SMTLIBNODE_H
#define YM_SMTLIBV2_SMTLIBNODE_H

/// @file ym_smtlibv2/SmtLibNode.h
/// @brief SmtLibNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_smtlibv2/smtlibv2_nsdef.h"
#include "ym_utils/FileRegion.h"


BEGIN_NAMESPACE_YM_SMTLIBV2

//////////////////////////////////////////////////////////////////////
/// @class SmtLibNode SmtLibNode.h "ym_smtlibv2/SmtLibNode.h"
/// @brief SmtLib のS式を表すノードの基底クラス
//////////////////////////////////////////////////////////////////////
class SmtLibNode
{
public:

  /// @brief デストラクタ
  virtual
  ~SmtLibNode() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが実装する仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を返す．
  virtual
  tTokenType
  type() const = 0;

  /// @brief ファイル上の位置を返す．
  virtual
  FileRegion
  loc() const = 0;

  /// @brief 終端型の場合の値を返す．
  virtual
  const char*
  value() const = 0;

  /// @brief LIST型の場合の子供のノードの要素数を返す．
  virtual
  ymuint
  child_num() const = 0;

  /// @brief LIST型の場合の子供のノードを返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < child_num() )
  virtual
  const SmtLibNode*
  child(ymuint pos) const = 0;

};

END_NAMESPACE_YM_SMTLIBV2

#endif // YM_SMTLIBV2_SMTLIBNODE_H