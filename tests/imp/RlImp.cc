
/// @file RlImp.cc
/// @brief RlImp の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012 Yusuke Matsunaga
/// All rights reserved.


#include "RlImp.h"
#include "StrImp.h"
#include "ImpMgr.h"
#include "StrNode.h"
#include "SnInput.h"
#include "SnAnd.h"
#include "SnXor.h"
#include "ImpInfo.h"
#include "ImpList.h"
#include "ym_networks/BdnMgr.h"
#include "ym_networks/BdnNode.h"
#include "ym_logic/SatSolver.h"


BEGIN_NAMESPACE_YM_NETWORKS

BEGIN_NONAMESPACE

bool debug = false;

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス RlImp
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
RlImp::RlImp()
{
  mLevel = 1;
}

// @brief デストラクタ
RlImp::~RlImp()
{
}

// @brief ラーニングのレベルを設定する．
void
RlImp::set_learning_level(ymuint level)
{
  mLevel = level;
}

// @brief ネットワーク中の間接含意を求める．
// @param[in] network 対象のネットワーク
// @param[in] imp_info 間接含意のリスト
void
RlImp::learning(const BdnMgr& network,
		ImpInfo& imp_info)
{
  ymuint n = network.max_node_id();

  imp_info.set_size(n);

  // BDN の情報を ImpMgr にコピーする．
  ImpMgr imp_mgr;
  imp_mgr.set(network);

#if 0
  imp_mgr.print_network(cout);
#endif

  for (ymuint i = 0; i < n; ++ i) {
    StrNode* node = imp_mgr.node(i);
    if ( node == NULL ) continue;

    ymuint src_id = node->id();

    // node に値を割り当てる．
    for (ymuint val = 0; val < 2; ++ val) {
      vector<ImpVal> imp_list;
      bool ok = make_all_implication(imp_mgr, node, val, mLevel, imp_list);
      if ( ok ) {
	imp_info.put(src_id, val, imp_list);
      }
    }
  }

#if 0
  // 直接含意を求める．
  StrImp strimp;
  ImpInfo d_imp;
  strimp.learning(network, d_imp);
  for (ymuint src_id = 0; src_id < n; ++ src_id) {
    for (ymuint src_val = 0; src_val < 2; ++ src_val) {
      list<ImpVal>& imp_list = imp_info.get(src_id, src_val);
      for (list<ImpVal>::iterator p = imp_list.begin();
	   p != imp_list.end(); ) {
	list<ImpVal>::iterator q = p;
	++ p;
	const ImpVal& imp = *q;
	ymuint dst_id = imp.id();
	ymuint dst_val = imp.val();
	if ( d_imp.check(src_id, src_val, dst_id, dst_val) ||
	     d_imp.check(dst_id, dst_val ^ 1, src_id, src_val ^ 1) ) {
	  imp_list.erase(q);
	}
      }
    }
  }
#endif

  // 検証
  if ( 0 ) {
    SatSolver solver;
    for (ymuint i = 0; i < n; ++ i) {
      VarId vid = solver.new_var();
      assert_cond( vid.val() == i, __FILE__, __LINE__);
    }
    vector<BdnNode*> node_list;
    network.sort(node_list);
    for (vector<BdnNode*>::iterator p = node_list.begin();
	 p != node_list.end(); ++ p) {
      const BdnNode* bnode = *p;
      ymuint id = bnode->id();
      Literal lit(VarId(id), kPolPosi);

      const BdnNode* bnode0 = bnode->fanin0();
      bool inv0 = bnode->fanin0_inv();
      Literal lit0(VarId(bnode0->id()), inv0 ? kPolNega : kPolPosi);

      const BdnNode* bnode1 = bnode->fanin1();
      bool inv1 = bnode->fanin1_inv();
      Literal lit1(VarId(bnode1->id()), inv1 ? kPolNega : kPolPosi);

      if ( bnode->is_and() ) {
	solver.add_clause(lit0, ~lit);
	solver.add_clause(lit1, ~lit);
	solver.add_clause(~lit0, ~lit1, lit);
      }
      else if ( bnode->is_xor() ) {
	solver.add_clause(lit0, ~lit1, lit);
	solver.add_clause(~lit0, lit1, lit);
	solver.add_clause(~lit0, ~lit1, ~lit);
	solver.add_clause(lit0, lit1, ~lit);
      }
      else {
	assert_not_reached(__FILE__, __LINE__);
      }
    }

    for (ymuint src_id = 0; src_id < n; ++ src_id) {
      for (ymuint src_val = 0; src_val < 2; ++ src_val) {
	Literal lit0(VarId(src_id), src_val == 0 ? kPolNega : kPolPosi);
	const ImpList& imp_list = imp_info.get(src_id, src_val);
	for (ImpList::iterator p = imp_list.begin();
	     p != imp_list.end(); ++ p) {
	  const ImpCell& imp = *p;
	  ymuint dst_id = imp.dst_id();
	  ymuint dst_val = imp.dst_val();
	  Literal lit1(VarId(dst_id), dst_val == 0 ? kPolNega : kPolPosi);
	  vector<Literal> tmp(2);
	  vector<Bool3> model;
	  tmp[0] = lit0;
	  tmp[1] = ~lit1;
	  if ( solver.solve(tmp, model) != kB3False ) {
	    cout << "Error: Node#" << src_id << ": " << src_val
		 << " ==> Node#" << dst_id << ": " << dst_val << endl;
	  }
	}
      }
    }
  }

#if 0
  cout << "RECURSIVE LERNING IMPLICATION" << endl;
  imp_info.print(cout);
#endif
}

// @brief recursive learning を行なう．
// @param[in] imp_mgr ImpMgr
// @param[in] node ノード
// @param[in] val 値
// @param[in] level レベル
// @param[in] imp_list 含意のリスト
bool
RlImp::make_all_implication(ImpMgr& imp_mgr,
			    StrNode* node,
			    ymuint val,
			    ymuint level,
			    vector<ImpVal>& imp_list)
{
  if ( debug ) {
    cout << "make_all_implication(Node#" << node->id()
	 << ": " << val << " @level#" << level << ")" << endl;
  }

  imp_list.clear();
  bool ok = imp_mgr.assert(node, val, imp_list);

  if ( debug ) {
    cout << "direct implications {" << endl;
    for (vector<ImpVal>::iterator p = imp_list.begin();
	 p != imp_list.end(); ++ p) {
      const ImpVal& imp = *p;
      ymuint dst_id = imp.id();
      ymuint dst_val = imp.val();
      cout << "  ==> Node#" << dst_id << ": " << dst_val << endl;
    }
    cout << "}" << endl;
  }

  if ( ok ) {
    if ( level > 0 ) {
      vector<StrNode*> unode_list;
      imp_mgr.get_unodelist(unode_list);
      // 共通の割り当てを調べるための配列
      vector<bool> vmark(imp_mgr.max_node_id(), false);
      for (vector<StrNode*>::iterator p = unode_list.begin();
	   p != unode_list.end(); ++ p) {
	StrNode* unode = *p;

	if ( debug ) {
	  cout << "Unode: Node#" << unode->id() << endl;
	}

	ymuint np = unode->justification_num();
	bool first = true;
	vector<ymuint> common_val(imp_mgr.max_node_id(), 2);
	vector<ymuint> common_list;
	for (ymuint i = 0; i < np; ++ i) {
	  ImpVal imp = unode->get_justification(i);
	  StrNode* inode = imp_mgr.node(imp.id());
	  ymuint ival = imp.val();

	  if ( debug ) {
	    cout << "  Inode: Node#" << inode->id()
		 << ": " << ival << endl;
	  }

	  vector<ImpVal> imp_list1;
	  bool ok1 = make_all_implication(imp_mgr, inode, ival, level - 1,
					  imp_list1);
	  if ( ok1 ) {
	    if ( first ) {
	      first = false;
	      for (vector<ImpVal>::iterator p = imp_list1.begin();
		   p != imp_list1.end(); ++ p) {
		const ImpVal& imp = *p;
		ymuint dst_id = imp.id();
		ymuint val = imp.val();
		if ( vmark[dst_id] ) {
		  common_val[dst_id] = 2;
		}
		else {
		  common_val[dst_id] = val;
		  common_list.push_back(dst_id);
		}
	      }
	    }
	    else {
	      vector<bool> vmark1(imp_mgr.max_node_id(), false);
	      for (vector<ImpVal>::iterator p = imp_list1.begin();
		   p != imp_list1.end(); ++ p) {
		const ImpVal& imp = *p;
		ymuint dst_id = imp.id();
		ymuint val = imp.val();
		if ( common_val[dst_id] != val ) {
		  common_val[dst_id] = 2;
		}
		vmark1[dst_id] = true;
	      }
	      for (vector<ymuint>::iterator p = common_list.begin();
		   p != common_list.end(); ++ p) {
		ymuint dst_id = *p;
		if ( !vmark1[dst_id] ) {
		  common_val[dst_id] = 2;
		}
	      }
	    }
	  }
	}
	ymuint nc = common_list.size();
	for (ymuint i = 0; i < nc; ++ i) {
	  ymuint dst_id = common_list[i];
	  if ( common_val[dst_id] == 2 ) continue;
	  imp_list.push_back(ImpVal(dst_id, common_val[dst_id]));

	  if ( debug ) {
	    cout << "  Common Implication: Node#" << dst_id
		 << ": " << common_val[dst_id] << endl;
	  }

	  vmark[dst_id] = true;
	}
	if ( debug ) {
	  cout << "Unode: Node#" << unode->id() << " end" << endl;
	}
      }
    }
  }
  else {
    // 単一の割り当てで矛盾が起こった．
    // node は !val 固定
  }
  imp_mgr.backtrack();

  return ok;
}

END_NAMESPACE_YM_NETWORKS