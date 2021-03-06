﻿#ifndef NETWORKS_BNETGATEDECOMP_H
#define NETWORKS_BNETGATEDECOMP_H

/// @file YmNetworks/BNetGateDecomp.h
/// @brief BNetGateDecomp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmNetworks/bnet.h"
#include "YmLogic/Expr.h"
#include "YmUtils/RandGen.h"


BEGIN_NAMESPACE_YM_NETWORKS_BNET

//////////////////////////////////////////////////////////////////////
/// @class BNetGateDecomp BNetGateDecomp.h "YmNetworks/BNetGateDecomp.h"
/// @ingroup BnetGroup
/// @brief BNetwork のノードの分解を行うクラス
/// @sa BNetwork BNetManip
//////////////////////////////////////////////////////////////////////
class BNetGateDecomp
{
public:
  //////////////////////////////////////////////////////////////////////
  // ゲートの型を表すビットマスク
  //////////////////////////////////////////////////////////////////////
  static
  const ymuint32 kAnd = 1U;
  static
  const ymuint32 kNand = 2U;
  static
  const ymuint32 kOr = 4U;
  static
  const ymuint32 kNor = 8U;
  static
  const ymuint32 kXor = 16U;
  static
  const ymuint32 kXnor = 32U;
  static
  const ymuint32 kBaseGrp = kAnd | kNand | kOr | kNor;
  static
  const ymuint32 kXorGrp = kXor | kXnor;


public:
  //////////////////////////////////////////////////////////////////////
  /// @name コンストラクタ/デストラクタ
  /// @{

  /// @brief コンストラクタ．
  BNetGateDecomp();

  /// @brief デストラクタ
  ~BNetGateDecomp();

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name ネットワークを分解する関数
  /// @{

  /// @brief 特定の型のみを用いた分解を行う．(バランス型)
  /// @param[in] network 操作対象のネットワーク
  /// @param[in] type_mask 使用可能なゲートの種類を表すビットマスク
  /// @param[in] max_fanin ファンインの最大数(0, 1で制限なし)
  /// @note 副作用として sweep() が行われる．
  void
  operator()(BNetwork& network,
	     ymuint32 type_mask,
	     ymuint max_fanin);

  /// @brief 特定の型のみを用いた分解を行う．(ランダム型)
  /// @param[in] network 操作対象のネットワーク
  /// @param[in] type_mask 使用可能なゲートの種類を表すビットマスク
  /// @param[in] max_fanin ファンインの最大数(0, 1で制限なし)
  /// @param[in] randgen 分解で用いる乱数発生器
  /// @note 副作用として sweep() が行われる．
  void
  operator()(BNetwork& network,
	     ymuint32 type_mask,
	     ymuint max_fanin,
	     RandGen& randgen);

  /// @}


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 特定の型のみを用いた分解を行う．
  /// @param[in] network 操作対象のネットワーク
  /// @param[in] type_mask 使用可能なゲートの種類を表すビットマスク
  void
  decomp_sub(BNetwork& network,
	     ymuint32 type_mask);

  /// @brief 必要となるインバーターの数を数える．
  ymuint
  count_inv(const Expr& expr,
	    BNode* node,
	    bool phase,
	    vector<bool>& iinv_array);

  // (node:inv) に対応するノードを返す．
  BNode*&
  node_map(BNode* node,
	   bool inv);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノードの対応表
  vector<BNode*> mNodeMap;

};

END_NAMESPACE_YM_NETWORKS_BNET

#endif // NETWORKS_BNETDECOMP_H
