﻿#ifndef CMNNODEPI_H
#define CMNNODEPI_H

/// @file CmnNodePI.h
/// @brief CmnNodePI のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "CmnNodeInput.h"


BEGIN_NAMESPACE_YM_NETWORKS_CMN

//////////////////////////////////////////////////////////////////////
/// @class CmnNodePI CmnNodePI.h "CmnNodePI.h"
/// @brief 外部入力を表す CmnNode の派生クラス
//////////////////////////////////////////////////////////////////////
class CmnNodePI :
  public CmnNodeInput
{
  friend class CmnMgrImpl;

private:

  /// @brief コンストラクタ
  /// @param[in] port 関連するポート
  /// @param[in] bitpos ポート中の位置
  CmnNodePI(const CmnPort* port,
	    ymuint bitpos);

  /// @brief デストラクタ
  virtual
  ~CmnNodePI();


public:

  /// @brief 入力ノードのサブタイプを得る．
  virtual
  tInputType
  input_type() const;

  /// @brief 関連するポートを返す．
  /// @note kPRIMARY_INPUT および kPRIMARY_OUTPUT の時のみ意味を持つ．
  /// @note それ以外では nullptr を返す．
  virtual
  const CmnPort*
  port() const;

  /// @brief ポート中のビット位置を返す．
  /// @note kPRIMARY_INPUT および kPRIMARY_OUTPUT の時のみ意味を持つ．
  /// @note それ以外では 0 を返す．
  virtual
  ymuint
  port_bitpos() const;

  /// @brief 入出力ノードの場合に相方のノードを返す．
  /// @note なければ nullptr を返す．
  virtual
  const CmnNode*
  alt_node() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 関連するポート
  const CmnPort* mPort;

  // ポート中のビット位置
  ymuint32 mBitPos;

};

END_NAMESPACE_YM_NETWORKS_CMN

#endif // CMNNODEPI_H
