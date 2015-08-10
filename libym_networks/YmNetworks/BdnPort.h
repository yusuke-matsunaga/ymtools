﻿#ifndef NETWORKS_BDNPORT_H
#define NETWORKS_BDNPORT_H

/// @file YmNetworks/BdnPort.h
/// @brief BdnPort のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmNetworks/bdn.h"


BEGIN_NAMESPACE_YM_NETWORKS_BDN

class BdnAuxData;

//////////////////////////////////////////////////////////////////////
/// @class BdnPort BdnPort.h "BdnPort.h"
/// @brief 外部入出力ポートを表すクラス
//////////////////////////////////////////////////////////////////////
class BdnPort
{
  friend class BdnMgrImpl;

private:

  /// @brief コンストラクタ
  BdnPort();

  /// @brief デストラクタ
  ~BdnPort();


public:

  /// @brief ID 番号を得る．
  ymuint
  id() const;

  /// @brief 名前を得る．
  string
  name() const;

  /// @brief ビット幅を得る．
  ymuint
  bit_width() const;

  /// @brief ビットごとの方向を得る．
  /// @param[out] iovect ビットごとの方向を収める配列
  /// @note iovect の値の意味は以下の通り
  ///  - 0 : なし
  ///  - 1 : 入力のみ
  ///  - 2 : 出力のみ
  ///  - 3 : 入力と出力
  void
  get_iovect(vector<ymuint>& iovect) const;

  /// @brief 入力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < bit_width )
  /// @note 出力ポートの場合には nullptr を返す．
  const BdnNode*
  input(ymuint pos) const;

  /// @brief 出力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < bit_width )
  /// @note 入力ポートの場合には nullptr を返す．
  const BdnNode*
  output(ymuint pos) const;

  /// @brief 入力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < bit_width )
  /// @note 出力ポートの場合には nullptr を返す．
  BdnNode*
  _input(ymuint pos);

  /// @brief 出力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < bit_width )
  /// @note 入力ポートの場合には nullptr を返す．
  BdnNode*
  _output(ymuint pos);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  ymuint32 mId;

  // 名前
  string mName;

  // ビット幅
  ymuint32 mBitWidth;

  // 入力ノードの配列
  BdnNode** mInputArray;

  // 出力ノードの配列
  BdnNode** mOutputArray;

  // BdnNode に付加する情報
  BdnAuxData** mAuxDataArray;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ID 番号を得る．
inline
ymuint
BdnPort::id() const
{
  return mId;
}

// @brief 名前を得る．
inline
string
BdnPort::name() const
{
  return mName;
}

// @brief ビット幅を得る．
inline
ymuint
BdnPort::bit_width() const
{
  return mBitWidth;
}

// @brief 入力ノードを得る．
// @param[in] pos 位置番号 ( 0 <= pos < bit_width )
// @note 出力ポートの場合には nullptr を返す．
inline
const BdnNode*
BdnPort::input(ymuint pos) const
{
  return mInputArray[pos];
}

// @brief 出力ノードを得る．
// @param[in] pos 位置番号 ( 0 <= pos < bit_width )
// @note 入力ポートの場合には nullptr を返す．
inline
const BdnNode*
BdnPort::output(ymuint pos) const
{
  return mOutputArray[pos];
}

// @brief 入力ノードを得る．
// @param[in] pos 位置番号 ( 0 <= pos < bit_width )
// @note 出力ポートの場合には nullptr を返す．
inline
BdnNode*
BdnPort::_input(ymuint pos)
{
  return mInputArray[pos];
}

// @brief 出力ノードを得る．
// @param[in] pos 位置番号 ( 0 <= pos < bit_width )
// @note 入力ポートの場合には nullptr を返す．
inline
BdnNode*
BdnPort::_output(ymuint pos)
{
  return mOutputArray[pos];
}

END_NAMESPACE_YM_NETWORKS_BDN

#endif // NETWORKS_BDNPORT_H
