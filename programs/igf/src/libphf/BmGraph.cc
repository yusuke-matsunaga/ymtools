
/// @file BmGraph.cc
/// @brief BmGraph の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#include "BmGraph.h"
#include "BmNode.h"
#include "BmEdge.h"


BEGIN_NAMESPACE_YM_IGF

//////////////////////////////////////////////////////////////////////
// クラス BmGraph
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] v1_num 節点集合 V1 の要素数
// @param[in] v2_num 節点集合 V2 の要素数
BmGraph::BmGraph(ymuint v1_num,
		 ymuint v2_num)
{
  mV1Num = v1_num;
  mV1Array = new BmNode[mV1Num];
  for (ymuint i = 0; i < mV1Num; ++ i) {
    mV1Array[i].set(0, i);
  }

  mV2Num = v2_num;
  mV2Array = new BmNode[mV2Num];
  for (ymuint i = 0; i < v2_num; ++ i) {
    mV2Array[i].set(1, i);
  }
}

// @brief デストラクタ
BmGraph::~BmGraph()
{
  delete [] mV1Array;
  delete [] mV2Array;

  for (vector<BmEdge*>::iterator p = mEdgeList.begin();
       p != mEdgeList.end(); ++ p) {
    delete *p;
  }
}

// @brief 節点集合 V1 の要素数を返す．
ymuint
BmGraph::v1_num()
{
  return mV1Num;
}

// @brief 節点集合 V1 の要素を返す．
// @param[in] pos 位置番号 ( 0 <= pos < v1_num() )
BmNode*
BmGraph::v1_elem(ymuint pos)
{
  assert_cond( pos < mV1Num, __FILE__, __LINE__);
  return &mV1Array[pos];
}

// @brief 節点集合 V2 の要素数を返す．
ymuint
BmGraph::v2_num()
{
  return mV2Num;
}

// @brief 節点集合 V2 の要素を返す．
// @param[in] pos 位置番号 ( 0 <= pos < v2_num() )
BmNode*
BmGraph::v2_elem(ymuint pos)
{
  assert_cond( pos < mV2Num, __FILE__, __LINE__);
  return &mV2Array[pos];
}

// @brief 枝数を返す．
ymuint
BmGraph::edge_num()
{
  return mEdgeList.size();
}

// @brief 枝を返す．
// @param[in] pos 位置番号 ( 0 <= pos < edge_num() )
BmEdge*
BmGraph::edge(ymuint pos)
{
  assert_cond( pos < mEdgeList.size(), __FILE__, __LINE__);
  return mEdgeList[pos];
}

// @brief 枝を作る．
// @param[in] v1 V1 側の要素
// @param[in] v2 V2 側の要素
// @note 重複チェックは行なわない．
BmEdge*
BmGraph::new_edge(BmNode* v1,
		  BmNode* v2)
{
  BmEdge* edge = new BmEdge(v1, v2);
  mEdgeList.push_back(edge);
  v1->mEdgeList.push_back(edge);
  v2->mEdgeList.push_back(edge);
  return edge;
}

// @breif 最大マッチングを求める．
bool
BmGraph::find_match(vector<BmEdge*>& edge_list)
{
  for (ymuint i = 0; i < mV1Num; ++ i) {
    BmNode* v1 = &mV1Array[i];
    ymuint ne = v1->edge_num();
    bool found = false;
    // 空いているノードを探す．
    for (ymuint j = 0; j < ne; ++ j) {
      BmEdge* edge = v1->edge(j);
      BmNode* v2 = edge->v2();
      BmEdge* edge1 = v2->mCurEdge;
      if ( edge1 == NULL ) {
	v1->mCurEdge = edge;
	v2->mCurEdge = edge;
	found = true;
	break;
      }
    }
    if ( found ) {
      continue;
    }

    // なかったので既に割り当てられている枝を取り替える．
    vector<bool> mark(mV1Num, false);
    if ( !find_alt_path(v1, mark) ) {
      return false;
    }
  }

  for (ymuint i = 0; i < mV1Num; ++ i) {
    BmNode* v1 = &mV1Array[i];
    BmEdge* edge = v1->mCurEdge;
    edge_list.push_back(edge);
  }
  return true;
}

// @brief 増加パスを見つける．
bool
BmGraph::find_alt_path(BmNode* v1,
		       vector<bool>& mark)
{
  if ( mark[v1->id()] ) {
    return false;
  }
  mark[v1->id()] = true;
  ymuint ne = v1->edge_num();
  for (ymuint i = 0; i < ne; ++ i) {
    BmEdge* edge = v1->edge(i);
    BmNode* v2 = edge->v2();
    BmEdge* edge1 = v2->mCurEdge;
    if ( edge1 == NULL ) {
      // 見つけた．
      v1->mCurEdge = edge;
      v2->mCurEdge = edge;
      return true;
    }
    if ( find_alt_path(edge1->v1(), mark) ) {
      // 見つけた．
      v1->mCurEdge = edge;
      v2->mCurEdge = edge;
      return true;
    }
  }
  return false;
}

END_NAMESPACE_YM_IGF
