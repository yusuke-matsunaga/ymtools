#ifndef LIBYM_TECHMAP_CELLMAP_LIBDUMP_LDFUNC_H
#define LIBYM_TECHMAP_CELLMAP_LIBDUMP_LDFUNC_H

/// @file libym_techmap/cellmap/libdump/LdFunc.h
/// @brief LdFunc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "LdCellGroup.h"
#include "ym_logic/NpnMap.h"


BEGIN_NAMESPACE_YM_CELLMAP_LIBDUMP

class LdFuncRep;

//////////////////////////////////////////////////////////////////////
/// @class LdFunc LdFunc.h "LdFunc.h"
/// @brief 論理関数を表すクラス
//////////////////////////////////////////////////////////////////////
class LdFunc :
  public LdCellGroup
{
  friend class LdFuncMgr;

private:

  /// @brief コンストラクタ
  LdFunc();

  /// @brief デストラクタ
  ~LdFunc();


public:

  /// @brief ID番号を返す．
  ymuint
  id() const;

  /// @brief 親の代表関数を返す．
  const LdFuncRep*
  rep() const;

  /// @brief 親に対する変換マップを返す．
  const NpnMap&
  map() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  ymuint32 mId;

  // 親の代表関数
  LdFuncRep* mRep;

  // 代表関数から自分自身へ変換するマップ
  NpnMap mMap;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
inline
LdFunc::LdFunc()
{
}

// @brief デストラクタ
inline
LdFunc::~LdFunc()
{
}

// @brief ID番号を返す．
inline
ymuint
LdFunc::id() const
{
  return mId;
}

// @brief 親の代表関数を返す．
inline
const LdFuncRep*
LdFunc::rep() const
{
  return mRep;
}

// @brief 親に対する変換マップを返す．
inline
const NpnMap&
LdFunc::map() const
{
  return mMap;
}

END_NAMESPACE_YM_CELLMAP_LIBDUMP

#endif // LIBYM_TECHMAP_CELLMAP_LIBDUMP_LDFUNC_H