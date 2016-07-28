#ifndef YM_NETWORKS_BDNBLIFREADER_H
#define YM_NETWORKS_BDNBLIFREADER_H

/// @file ym_networks/BdnBlifReader.h
/// @brief BdnBlifReader のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_networks/bdn.h"
#include "ym_cell/cell_nsdef.h"


BEGIN_NAMESPACE_YM_NETWORKS_BDN

//////////////////////////////////////////////////////////////////////
/// @class BdnBlifReader BdnBlifReader.h "ym_networks/BdnBlifReader.h"
/// @ingroup BdnGroup
/// @brief blif 形式のファイルを読み込んで BDN に設定するクラス
/// @sa BdnMgr nsYm::nsBlif::BlifParser
//////////////////////////////////////////////////////////////////////
class BdnBlifReader
{
public:

  /// @brief コンストラクタ
  BdnBlifReader();

  /// @brief デストラクタ
  ~BdnBlifReader();


public:

  /// @brief blif 形式のファイルを読み込む
  /// @param[in] filename ファイル名
  /// @param[in] network 読み込んだ内容を設定するネットワーク
  /// @param[in] cell_library セルライブラリ
  /// @retval true 正常に読み込めた
  /// @retval false 読み込み中にエラーが起こった．
  bool
  operator()(const string& filename,
	     BdnMgr& network,
	     const CellLibrary* cell_library = NULL);

};

END_NAMESPACE_YM_NETWORKS_BDN

#endif // YM_NETWORKS_BDNBLIFREADER_H