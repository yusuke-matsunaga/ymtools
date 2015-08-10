﻿
/// @file BNetGateDecomp.cc
/// @brief BNetGateDecomp の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmNetworks/BNetGateDecomp.h"
#include "YmNetworks/BNetManip.h"
#include "YmNetworks/BNetDecomp.h"


BEGIN_NAMESPACE_YM_NETWORKS_BNET

// @brief コンストラクタ．
BNetGateDecomp::BNetGateDecomp()
{
}

// @brief デストラクタ
BNetGateDecomp::~BNetGateDecomp()
{
}

// @brief 特定の型のみを用いた分解を行う．(バランス型)
// @param[in] network 操作対象のネットワーク
// @param[in] type_mask 使用可能なゲートの種類を表すビットマスク
// @param[in] max_fanin ファンインの最大数(0, 1で制限なし)
// @note 副作用として sweep() が行われる．
void
BNetGateDecomp::operator()(BNetwork& network,
			   ymuint32 type_mask,
			   ymuint max_fanin)
{
  BNetDecomp decomp;

  bool no_xor = ((type_mask & (kXor | kXnor)) == 0U);
  decomp(network, max_fanin, no_xor);

  decomp_sub(network, type_mask);
}

// @brief 特定の型のみを用いた分解を行う．(ランダム型)
// @param[in] network 操作対象のネットワーク
// @param[in] type_mask 使用可能なゲートの種類を表すビットマスク
// @param[in] max_fanin ファンインの最大数(0, 1で制限なし)
// @param[in] randgen 分解で用いる乱数発生器
// @note 副作用として sweep() が行われる．
void
BNetGateDecomp::operator()(BNetwork& network,
			   ymuint32 type_mask,
			   ymuint max_fanin,
			   RandGen& randgen)
{
  BNetDecomp decomp;

  bool no_xor = ((type_mask & (kXor | kXnor)) == 0U);
  decomp(network, max_fanin, randgen, no_xor);

  decomp_sub(network, type_mask);
}

// @brief 特定の型のみを用いた分解を行う．
// @param[in] network 操作対象のネットワーク
// @param[in] type_mask 使用可能なゲートの種類を表すビットマスク
void
BNetGateDecomp::decomp_sub(BNetwork& network,
			   ymuint32 type_mask)
{
  ymuint n = network.max_node_id();
  mNodeMap.clear();
  mNodeMap.resize(n * 2, nullptr);

  // 入力ノードとFFノードを mNodeMap に設定する．
  for (BNodeList::const_iterator p = network.inputs_begin();
       p != network.inputs_end(); ++ p) {
    BNode* node = *p;
    node_map(node, false) = node;
  }
  for (BNodeList::const_iterator p = network.latch_nodes_begin();
       p != network.latch_nodes_end(); ++ p) {
    BNode* node = *p;
    node_map(node, false) = node;
  }

  BNetManip manip(&network);

  // 論理ノードを type_mask に応じて変換する．
  BNodeVector node_list;
  network.tsort(node_list);
  for (BNodeVector::iterator p = node_list.begin(); p != node_list.end(); ++ p) {
    BNode* node = *p;
    Expr expr = node->func();
    if ( expr.is_literal() || expr.is_constant() ) {
      continue;
    }

    ymuint ni = node->fanin_num();
    ASSERT_COND(expr.child_num() == ni );

    ymuint best_cost = ni + 2;
    ymuint32 best_type = 0;
    vector<bool> best_iinv(ni);
    bool best_oinv = false;
    if ( expr.is_and() ) {
      if ( type_mask & kAnd ) {
	vector<bool> iinv(ni);
	ymuint cost = count_inv(expr, node, false, iinv);
	if ( best_cost > cost ) {
	  best_cost = cost;
	  best_type = kAnd;
	  best_iinv = iinv;
	  best_oinv = false;
	}
      }
      if ( type_mask & kNand ) {
	vector<bool> iinv(ni);
	ymuint cost = count_inv(expr, node, false, iinv) + 1;
	if ( best_cost > cost ) {
	  best_cost = cost;
	  best_type = kNand;
	  best_iinv = iinv;
	  best_oinv = true;
	}
      }
      if ( type_mask & kOr ) {
	vector<bool> iinv(ni);
	ymuint cost = count_inv(expr, node, true, iinv) + 1;
	if ( best_cost > cost ) {
	  best_cost = cost;
	  best_type = kOr;
	  best_iinv = iinv;
	  best_oinv = true;
	}
      }
      if ( type_mask & kNor ) {
	vector<bool> iinv(ni);
	ymuint cost = count_inv(expr, node, true, iinv);
	if ( best_cost > cost ) {
	  best_cost = cost;
	  best_type = kNor;
	  best_iinv = iinv;
	  best_oinv = false;
	}
      }
    }
    else if ( expr.is_or() ) {
      if ( type_mask & kAnd ) {
	vector<bool> iinv(ni);
	ymuint cost = count_inv(expr, node, true, iinv) + 1;
	if ( best_cost > cost ) {
	  best_cost = cost;
	  best_type = kAnd;
	  best_iinv = iinv;
	  best_oinv = true;
	}
      }
      if ( type_mask & kNand ) {
	vector<bool> iinv(ni);
	ymuint cost = count_inv(expr, node, true, iinv);
	if ( best_cost > cost ) {
	  best_cost = cost;
	  best_type = kNand;
	  best_iinv = iinv;
	  best_oinv = false;
	}
      }
      if ( type_mask & kOr ) {
	vector<bool> iinv(ni);
	ymuint cost = count_inv(expr, node, false, iinv);
	if ( best_cost > cost ) {
	  best_cost = cost;
	  best_type = kOr;
	  best_iinv = iinv;
	  best_oinv = false;
	}
      }
      if ( type_mask & kNor ) {
	vector<bool> iinv(ni);
	ymuint cost = count_inv(expr, node, false, iinv) + 1;
	if ( best_cost > cost ) {
	  best_cost = cost;
	  best_type = kNor;
	  best_iinv = iinv;
	  best_oinv = true;
	}
      }
    }
    else if ( expr.is_xor() ) {
      // XOR/XNOR の場合には個々の入力の極性を区別する必要はない．
      ymuint inv = 0;
      for (ymuint i = 0; i < ni; ++ i) {
	if ( expr.child(i).is_negaliteral() ) {
	  inv ^= 1;
	}
      }
      if ( type_mask & kXor ) {
	if ( inv == 0 ) {
	  // そのまま
	  best_cost = 0;
	  best_type = kXor;
	  for (ymuint i = 0; i < ni; ++ i) {
	    best_iinv[i] = false;
	  }
	  best_oinv = false;
	}
	else {
	  // 逆極性がコスト0で使用可能な入力を探す．
	  ymuint inv_pos = ni;
	  for (ymuint i = 0; i < ni; ++ i) {
	    BNode* node1 = node->fanin(i);
	    if ( node_map(node1, true) ) {
	      inv_pos = i;
	      break;
	    }
	  }
	  if ( inv_pos < ni ) {
	    best_cost = 0;
	    best_type = kXor;
	    for (ymuint i = 0; i < ni; ++ i) {
	      best_iinv[i] = (i == inv_pos);
	    }
	    best_oinv = false;
	  }
	  else {
	    best_cost = 1;
	    best_type = kXor;
	    for (ymuint i = 0; i < ni; ++ i) {
	      best_iinv[i] = false;
	    }
	    best_oinv = true;
	  }
	}
      }
      if ( best_cost > 0 && type_mask & kXnor ) {
	if ( inv == 1 ) {
	  // そのまま
	  best_cost = 0;
	  best_type = kXnor;
	  for (ymuint i = 0; i < ni; ++ i) {
	    best_iinv[i] = false;
	  }
	  best_oinv = false;
	}
	else {
	  // 逆極性がコスト0で使用可能な入力を探す．
	  ymuint inv_pos = ni;
	  for (ymuint i = 0; i < ni; ++ i) {
	    BNode* node1 = node->fanin(i);
	    if ( node_map(node1, true) ) {
	      inv_pos = i;
	      break;
	    }
	  }
	  if ( inv_pos < ni ) {
	    best_cost = 0;
	    best_type = kXnor;
	    for (ymuint i = 0; i < ni; ++ i) {
	      best_iinv[i] = (i == inv_pos);
	    }
	    best_oinv = false;
	  }
	}
      }
    }
    ASSERT_COND(best_cost < ni + 2 );

    BNodeVector new_fanins(ni);
    for (ymuint i = 0; i < ni; ++ i) {
      BNode* node1 = node->fanin(i);
      BNode* new_node = nullptr;
      if ( best_iinv[i] ) {
	new_node = node_map(node1, true);
	if ( new_node == nullptr ) {
	  BNode* node0 = node_map(node1, false);
	  new_node = manip.make_inverter(node0);
	}
      }
      else {
	new_node = node_map(node1, false);
	ASSERT_COND(new_node );
      }
      new_fanins[i] = new_node;
    }

    ExprVector lit_array(ni);
    for (ymuint i = 0; i < ni;  ++ i) {
      lit_array[i] = Expr::make_posiliteral(VarId(i));
    }
    Expr new_expr;
    if ( best_type == kAnd ) {
      new_expr = Expr::make_and(lit_array);
    }
    else if ( best_type == kNand ) {
      new_expr = ~Expr::make_and(lit_array);
    }
    else if ( best_type == kOr ) {
      new_expr = Expr::make_or(lit_array);
    }
    else if ( best_type == kNor ) {
      new_expr = ~Expr::make_or(lit_array);
    }
    else if ( best_type == kXor ) {
      new_expr = Expr::make_xor(lit_array);
    }
    else if ( best_type == kXnor ) {
      new_expr = ~Expr::make_xor(lit_array);
    }
    else {
      ASSERT_NOT_REACHED;
    }
    if ( best_oinv ) {
      BNode* new_node = manip.new_logic();
      bool stat1 = manip.change_logic(new_node, new_expr, new_fanins);
      ASSERT_COND( stat1 );
      node_map(node, true) = new_node;
      bool stat2 = manip.change_to_inverter(node, new_node);
      ASSERT_COND( stat2 );
    }
    else {
      bool stat = manip.change_logic(node, new_expr, new_fanins);
      ASSERT_COND( stat );
    }
    node_map(node, false) = node;
  }
  mNodeMap.clear();

  // 使われなかったノードを取り除く
  network.sweep();
}

// expr の子供がすべてリテラルだと仮定して必要となるインバーターの数を数える．
ymuint
BNetGateDecomp::count_inv(const Expr& expr,
			  BNode* node,
			  bool phase,
			  vector<bool>& iinv_array)
{
  ymuint nc = expr.child_num();
  ymuint c = 0;
  for (ymuint i = 0; i < nc; ++ i) {
    Expr expr1 = expr.child(i);
    VarId var = expr1.varid();
    ymuint pos = var.val();
    BNode* node1 = node->fanin(pos);
    bool inv = false;
    if ( phase & expr1.is_posiliteral() ||
	 ~phase & expr1.is_negaliteral() ) {
      inv = true;
      iinv_array[pos] = true;
    }
    else {
      iinv_array[pos] = false;
    }
    if ( node_map(node1, inv) == nullptr ) {
      ++ c;
    }
  }
  return c;
}

// (node:inv) に対応するノードを返す．
BNode*&
BNetGateDecomp::node_map(BNode* node,
			 bool inv)
{
  ymuint offset = (inv) ? 1 : 0;
  return mNodeMap[node->id() * 2 + offset];
}

END_NAMESPACE_YM_NETWORKS_BNET
