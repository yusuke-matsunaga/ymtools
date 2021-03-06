﻿
/// @file LtConv.cc
/// @brief LtConv の実装クラス
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "LtConv.h"
#include "YmNetworks/MvnNode.h"
#include "YmNetworks/MvnBdnMap.h"
#include "YmNetworks/BdnMgr.h"
#include "YmNetworks/BdnNodeHandle.h"


BEGIN_NAMESPACE_YM_NETWORKSBDNCONV

// @brief コンストラクタ
LtConv::LtConv()
{
}

// @brief デストラクタ
LtConv::~LtConv()
{
}

// @brief MvnNode を BdnMgr に変換する．
// @param[in] node ノード
// @param[in] bdnetwork 変換結果の BdnMgr
// @param[in] nodemap ノードの対応関係を表すマップ
// @retval true このクラスで変換処理を行った．
// @retval false このクラスでは変換処理を行わなかった．
bool
LtConv::operator()(const MvnNode* node,
		   BdnMgr& bdnetwork,
		   MvnBdnMap& nodemap)
{
  if ( node->type() == MvnNode::kLt ) {
    const MvnInputPin* ipin0 = node->input(0);
    const MvnNode* src_node0 = ipin0->src_node();

    const MvnInputPin* ipin1 = node->input(1);
    const MvnNode* src_node1 = ipin1->src_node();

    ymuint bw = src_node0->bit_width();
    ASSERT_COND( src_node1->bit_width() == bw );

    BdnNodeHandle lt(BdnNodeHandle::make_zero());
    for (ymuint i = 0; i < bw; ++ i) {
      BdnNodeHandle a = nodemap.get(src_node0, i);
      BdnNodeHandle b = nodemap.get(src_node1, i);
      BdnNodeHandle p = bdnetwork.new_xnor(a, b);
      BdnNodeHandle g = bdnetwork.new_and(~a, b);
      BdnNodeHandle tmp = bdnetwork.new_and(lt, p);
      lt = bdnetwork.new_or(g, tmp);
    }
    nodemap.put(node, 0, lt);

    return true;
  }
  return false;
}

END_NAMESPACE_YM_NETWORKSBDNCONV
