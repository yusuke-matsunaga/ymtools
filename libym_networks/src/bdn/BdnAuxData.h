﻿#ifndef BDNAUXDATA_H
#define BDNAUXDATA_H

/// @file BdnAuxData.h
/// @brief BdnAuxData のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmNetworks/bdn.h"


BEGIN_NAMESPACE_YM_NETWORKS_BDN

//////////////////////////////////////////////////////////////////////
/// @class BdnAuxData BdnAuxData.h "BdnAuxData.h"
/// @brief 補助的なデータを表すクラス
//////////////////////////////////////////////////////////////////////
class BdnAuxData
{
  friend class BdnMgrImpl;

protected:

  /// @brief デストラクタ
  virtual
  ~BdnAuxData() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 派生クラスが実装する仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 関連するポートを返す．
  /// @note kINPUT および kOUTPUT の時に意味を持つ．
  /// @note それ以外では nullptr を返す．
  virtual
  const BdnPort*
  port() const;

  /// @brief ポート中の位置を返す．
  /// @note kINPUT および kOUTPUT の時に意味を持つ．
  /// @note それ以外では 0 を返す．
  virtual
  ymuint
  port_bitpos() const;

  /// @brief 関連する D-FF を返す．
  /// @note D-FF に関連していない場合には nullptr を返す．
  virtual
  const BdnDff*
  dff() const;

  /// @brief 関連するラッチを返す．
  /// @note ラッチに関連していない場合には nullptr を返す．
  virtual
  const BdnLatch*
  latch() const;

};


//////////////////////////////////////////////////////////////////////
/// @class BdnPortData BdnAuxData.h "BdnAuxData.h"
/// @brief ポートに関連した情報を表すクラス
//////////////////////////////////////////////////////////////////////
class BdnPortData :
  public BdnAuxData
{
  friend class BdnMgrImpl;

private:

  /// @brief コンストラクタ
  /// @param[in] port ポート
  /// @param[in] bitpos ビット位置
  BdnPortData(BdnPort* port,
	      ymuint bitpos);

  /// @brief デストラクタ
  virtual
  ~BdnPortData();


public:
  //////////////////////////////////////////////////////////////////////
  // BdnAuxData の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 関連するポートを返す．
  /// @note kINPUT および kOUTPUT の時に意味を持つ．
  /// @note それ以外では nullptr を返す．
  virtual
  const BdnPort*
  port() const;

  /// @brief ポート中の位置を返す．
  /// @note kINPUT および kOUTPUT の時に意味を持つ．
  /// @note それ以外では 0 を返す．
  virtual
  ymuint
  port_bitpos() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ポート
  BdnPort* mPort;

  // ビット位置
  ymuint32 mBitPos;

};


//////////////////////////////////////////////////////////////////////
/// @class BdnDffData BdnAuxData.h "BdnAuxData.h"
/// @brief D-FF に関連した情報を表すクラス
//////////////////////////////////////////////////////////////////////
class BdnDffData :
  public BdnAuxData
{
  friend class BdnMgrImpl;

private:

  /// @brief コンストラクタ
  /// @param[in] dff D-FF
  BdnDffData(BdnDff* dff);

  /// @brief デストラクタ
  virtual
  ~BdnDffData();


public:
  //////////////////////////////////////////////////////////////////////
  // BdnAuxData の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 関連する D-FF を返す．
  /// @note D-FF に関連していない場合には nullptr を返す．
  virtual
  const BdnDff*
  dff() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // D-FF
  BdnDff* mDff;

};


//////////////////////////////////////////////////////////////////////
/// @class BdnLatchData BdnAuxData.h "BdnAuxData.h"
/// @brief ラッチに関連した情報を表すクラス
//////////////////////////////////////////////////////////////////////
class BdnLatchData :
  public BdnAuxData
{
  friend class BdnMgrImpl;

private:

  /// @brief コンストラクタ
  /// @param[in] latch ラッチ
  BdnLatchData(BdnLatch* latch);

  /// @brief デストラクタ
  virtual
  ~BdnLatchData();


public:
  //////////////////////////////////////////////////////////////////////
  // BdnAuxData の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 関連するラッチを返す．
  /// @note ラッチに関連していない場合には nullptr を返す．
  virtual
  const BdnLatch*
  latch() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ラッチ
  BdnLatch* mLatch;

};

END_NAMESPACE_YM_NETWORKS_BDN

#endif // BDNAUXDATA_H
