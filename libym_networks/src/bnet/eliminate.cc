﻿
/// @file eliminate.cc
/// @brief eliminate() の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmNetworks/BNetwork.h"
#include "YmNetworks/BNetManip.h"


BEGIN_NAMESPACE_YM_NETWORKS_BNET


BEGIN_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// eliminate のノード整列用の構造体
//////////////////////////////////////////////////////////////////////
struct ElimElem
{
  // ノード
  BNode* mNode;

  // 入力からのレベル
  int mLevel;

  // ファンアウト先のリテラルの出現頻度
  int mOvalue;

  // コンストラクタ
  ElimElem(BNode* node = nullptr,
	   int level = 0,
	   int ovalue = 0) :
    mNode(node),
    mLevel(level),
    mOvalue(ovalue)
  {
  }

  // ソート用の比較関数
  friend
  bool
  operator<(const ElimElem& a,
	    const ElimElem& b)
  {
    if (a.mLevel < b.mLevel) {
      return true;
    }
    if (a.mLevel > b.mLevel) {
      return false;
    }
    return a.mOvalue < b.mOvalue;
  }
};


// eliminate 用にノードのソートを行う．
void
sort_nodes(const BNetwork* network,
	   BNodeVector& node_vec)
{
  ymuint n = network->logic_node_num();
  node_vec.reserve(n);

  vector<ElimElem> work;
  work.reserve(n);

  ymuint max_id = network->max_node_id();
  vector<ymuint> level_map(max_id, 0);

  // 外部入力のレベルは 0
  for (BNodeList::const_iterator p = network->inputs().begin();
       p != network->inputs().end(); ++ p) {
    BNode* node = *p;
    level_map[node->id()] = 0;
  }

  // まず，トポロジカル順にノードとその情報を work に入れる．
  node_vec.clear();
  network->tsort(node_vec);
  for (ymuint i = 0; i < n; i ++) {
    BNode* node = node_vec[i];
    ymuint ni = node->fanin_num();
    int level = 0;
    for (ymuint j = 0; j < ni; j ++) {
      BNode* fanin = node->fanin(j);
      int ilevel = level_map[fanin->id()];
      if ( level < ilevel ) {
	level = ilevel;
      }
    }
    level ++;
    level_map[node->id()] = level;
    int ovalue = 0;
    for (BNodeFoList::const_iterator p = node->fanouts_begin();
	 p != node->fanouts_end(); ++ p) {
      BNodeEdge* edge = *p;
      BNode* onode = edge->to();
      if ( onode->is_output() ) {
	ovalue = INT_MAX;
	break;
      }
      // ファンアウト先のファクタードフォームでの出現頻度を求める．
      int a = onode->func().litnum(VarId(edge->pos()));
      ovalue += a;
    }
    work.push_back(ElimElem(node, level, ovalue));
  }

  // level と ovalue にもとづいてソートする．
  sort(work.begin(), work.end());

  // 結果のノードを node_vec に移す．
  node_vec.clear();
  for (vector<ElimElem>::const_iterator p = work.begin();
       p != work.end(); ++ p) {
    node_vec.push_back(p->mNode);
  }
}

END_NONAMESPACE


// @brief 「価値」がしきい値以下のノードを削除する．
// @param[in] threshold しきい値
// @param[in] sop_limit SOP のリテラル数の増分の上限
// @param[in] auto_limit 上限を自動計算するとき true
// @note sop_limit が 0 のとき，上限なし
// @note sop_limit と auto_limit がともに指定されたら小さい方を用いる．
void
BNetwork::eliminate(int threshold,
		    ymuint sop_limit,
		    bool auto_limit)
{
  if ( sop_limit == 0 ) {
    sop_limit = UINT_MAX;
  }

  BNetManip manip(this);

  // ノードを一時的に格納するベクタを確保する．
  // 現在のノード数よりも増えることはないので
  // 現在のノード数だけ確保しておく．
  ymuint n = logic_node_num();
  vector<BNode*> node_vec;
  node_vec.reserve(n);

  // 消去されるノードがあるかぎり以下のループを繰り返す．
  bool eliminated = false;
  do {
    // 最初はなんにも消去されていない．
    eliminated = false;

    // まずどこにもファンアウトしていないノードを削除する．
    clean_up();

    // ノードを入力からのレベルとファンアウト先の出現頻度で
    // ソートする．
    // 結果は node_vec に入る．
    sort_nodes(this, node_vec);

    if ( auto_limit ) {
      // よくわかんないけど sis では現在のSOPサイズの2倍を
      // 上限ときめているようだ．
      ymuint max_cube = 0;
      for (vector<BNode*>::const_iterator p = node_vec.begin();
	   p != node_vec.end(); ++ p) {
	BNode* node = *p;
	ymuint cube_num = node->func().sop_cubenum();
	if ( max_cube < cube_num ) {
	  max_cube = cube_num;
	}
      }
      max_cube *= 2;
      if ( sop_limit > max_cube ) {
	sop_limit = max_cube;
      }
    }

    for (vector<BNode*>::const_iterator p = node_vec.begin();
	 p != node_vec.end(); ++ p) {
      BNode* node = *p;

      int value = node->value();
      if ( value > threshold ) {
	// このノードは「価値」があるので消去しない．
	continue;
      }

      // 価値が thresh 以下でもつぎの場合には消去しない．
      // 1. 外部出力にファンアウトしている．
      // 2. 消去するとファンアウト先のノードのSOPのキューブ数が
      //    sop_limit を越える．
      const Expr& func = node->func();
      // 肯定のSOPのキューブ数 - 1 (増分)
      ymuint pc = func.sop_cubenum() - 1;
      // 否定のSOPのキューブ数 - 1 (増分)
      ymuint nc = (~func).sop_cubenum() - 1;

      bool check = true;
      for (BNodeFoList::const_iterator p = node->fanouts_begin();
	   p != node->fanouts_end(); ++ p) {
	BNodeEdge* edge = *p;
	BNode* onode = edge->to();
	if ( onode->is_output() ) {
	  check = false;
	  break;
	}
	const Expr& ofunc = onode->func();
	// SOP形式で肯定のリテラルの現れる回数
	ymuint pa = ofunc.sop_litnum(VarId(edge->pos()), false);
	// SOP形式で否定のリテラルの現れる回数
	ymuint na = ofunc.sop_litnum(VarId(edge->pos()), true);
	// それらに肯定および否定のSOPのキューブ数をかける．
	ymuint c = pa * pc + na * nc;
	if ( ofunc.sop_cubenum() + c > sop_limit ) {
	  check = false;
	  break;
	}
      }

      if ( check ) {
	manip.eliminate_node(node);
	eliminated = true;
      }
    }

  } while ( eliminated );
}

END_NAMESPACE_YM_NETWORKS_BNET
