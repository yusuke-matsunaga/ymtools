﻿
/// @file BNetEquWriter.cc
/// @brief BNetEquWriter の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmNetworks/BNetEquWriter.h"
#include "YmNetworks/BNetwork.h"
#include "YmLogic/ExprWriter.h"


BEGIN_NAMESPACE_YM_NETWORKS_BNET

BEGIN_NONAMESPACE

// 論理式を出力する．
void
write_expr(const Expr& f,
	   const ExprWriter& writer,
	   const BNode* node,
	   ostream& s)
{
  HashMap<VarId, string> fanins;
  for (ymuint i = 0; i < node->fanin_num(); i ++) {
    BNode* inode = node->fanin(i);
    ASSERT_COND(inode != 0 );
    fanins.add(VarId(i), inode->name());
  }
  writer.dump(s, f, fanins);
}

// node のファンアウトの中に外部出力がただ一つ，含まれていたら
// そのノードをとってくる．
// 外部出力が全く含まれない場合および複数含まれる場合には nullptr
// を返す．
const BNode*
po_node(const BNode* node)
{
  const BNode* onode = 0;
  for (BNodeFoList::const_iterator p = node->fanouts_begin();
       p != node->fanouts_end(); ++p) {
    BNodeEdge* edge = *p;
    BNode* tmp = edge->to();
    if ( tmp->is_output() ) {
      // 外部出力にファンアウトしていた．
      if ( onode ) {
	// 複数の外部出力にファンアウトしていた．
	onode = 0;
	break;
      }
      onode = tmp;
    }
  }
  return onode;
}

// node の名前を取ってくる．
// nodeの唯一のファンアウトが外部出力の場合には，
// 自分の名前の代わりに外部出力の名前を使う．
string
node_name(const BNode* node)
{
  string name = node->name();
  const BNode* onode = po_node(node);
  if ( onode ) {
    // ファンアウト中に外部出力がただ一つだけ含まれる場合には
    // その名前を直接使う．
    name = onode->name();
  }
  return name;
}

END_NONAMESPACE


// @brief コンストラクタ
BNetEquWriter::BNetEquWriter()
{
}

// @brief デストラクタ
BNetEquWriter::~BNetEquWriter()
{
}

// ブーリアンネットワークを論理式の形で表示する．
void
BNetEquWriter::dump(ostream& s,
		    const BNetwork& network) const
{
  s << ".inputs";
  for (BNodeList::const_iterator p = network.inputs_begin();
       p != network.inputs_end(); ++p) {
    BNode* node = *p;
    s << " " << node->name();
  }
  s << endl;

  s << ".outputs";
  for (BNodeList::const_iterator p = network.outputs_begin();
       p != network.outputs_end(); ++p) {
    BNode* node = *p;
    s << " " << node->name() << "=" << node->fanin(0)->name();
  }
  s << endl;

  ExprWriter writer;
  for (BNodeList::const_iterator p = network.logic_nodes_begin();
       p != network.logic_nodes_end(); ++p) {
    BNode* node = *p;
    s << node_name(node) << " = ";
    write_expr(node->func(), writer, node, s);
    s << ";" << endl;
  }
}

END_NAMESPACE_YM_NETWORKS_BNET
