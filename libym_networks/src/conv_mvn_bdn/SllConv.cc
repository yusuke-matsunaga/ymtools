﻿
/// @file SllConv.cc
/// @brief SllConv の実装クラス
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "SllConv.h"
#include "YmNetworks/MvnNode.h"
#include "YmNetworks/MvnBdnMap.h"
#include "YmNetworks/BdnMgr.h"
#include "YmNetworks/BdnNodeHandle.h"


BEGIN_NAMESPACE_YM_NETWORKSBDNCONV

// @brief コンストラクタ
SllConv::SllConv()
{
}

// @brief デストラクタ
SllConv::~SllConv()
{
}

// @brief MvnNode を BdnMgr に変換する．
// @param[in] node ノード
// @param[in] bdnetwork 変換結果のサブジェクトグラフ
// @param[in] nodemap ノードの対応関係を表すマップ
// @retval true このクラスで変換処理を行った．
// @retval false このクラスでは変換処理を行わなかった．
bool
SllConv::operator()(const MvnNode* node,
		    BdnMgr& bdnetwork,
		    MvnBdnMap& nodemap)
{
  if ( node->type() == MvnNode::kSll ) {
    cerr << "Converter for SLL is not implemented" << endl;
#if 0
    const MvInputPin* ipin0 = node->input(0);
    const MvnNode* src_node0 = ipin0->src_node();

    const MvInputPin* ipin1 = node->input(1);
    const MvnNode* src_node1 = ipin1->src_node();

    ymuint bw = node->bit_width();
    ASSERT_COND( src_node0->bit_width() == bw );
    ASSERT_COND( src_node1->bit_width() == bw );
    for (ymuint i = 0; i < bw; ++ i) {
      BdnNodeHandle handle0 = nodemap.get(src_node0, i);
      BdnNodeHandle handle1 = nodemap.get(src_node1, i);
      BdnNodeHandle handle = bdnetwork.new_and(handle0, handle1);
      nodemap.put(node, i, handle);
    }
#endif
    return true;
  }
  return false;
}

END_NAMESPACE_YM_NETWORKSBDNCONV
