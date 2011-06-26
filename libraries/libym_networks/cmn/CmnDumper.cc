
/// @file libym_networks/cmn/CmnDumper.cc
/// @brief CmnDumper 実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_networks/CmnDumper.h"
#include "ym_networks/CmnMgr.h"
#include "ym_networks/CmnPort.h"
#include "ym_networks/CmnDff.h"
#include "ym_networks/CmnLatch.h"
#include "ym_networks/CmnNode.h"
#include "ym_networks/CmnDffCell.h"
#include "ym_networks/CmnLatchCell.h"
#include "ym_cell/Cell.h"
#include "ym_cell/CellPin.h"


BEGIN_NAMESPACE_YM_NETWORKS

// @brief コンストラクタ
CmnDumper::CmnDumper()
{
}

// @brief デストラクタ
CmnDumper::~CmnDumper()
{
}

void
CmnDumper::dump(ostream& s,
		const CmnMgr& network)
{
  ymuint np = network.port_num();
  for (ymuint i = 0; i < np; ++ i) {
    const CmnPort* port = network.port(i);
    s << "PORT#" << i << "(" << port->name() << "): ";

    ymuint nb = port->bit_width();
    assert_cond( nb > 0, __FILE__, __LINE__);
    if ( nb == 1 ) {
      const CmnNode* input = port->input(0);
      const CmnNode* output = port->output(0);
      if ( input ) {
	s << " INPUT(" << input->id_str() << ")";
      }
      if ( output ) {
	s << " OUTPUT(" << output->id_str() << ") = "
	  << output->fanin(0)->id_str();
      }
    }
    else {
      s << "{" << endl;
      for (ymuint j = 0; j < nb; ++ j) {
	ymuint idx = nb - j - 1;
	const CmnNode* input = port->input(idx);
	const CmnNode* output = port->output(idx);
	s << "    ";
	if ( input ) {
	  s << " INPUT(" << input->id_str() << ")";
	}
	if ( output ) {
	  s << " OUTPUT(" << output->id_str() << ") = "
	    << output->fanin(0)->id_str();
	}
	s << endl;
      }
      s << "}";
    }
    s << endl;
  }

  const CmnDffList& dff_list = network.dff_list();
  for (CmnDffList::const_iterator p = dff_list.begin();
       p != dff_list.end(); ++ p) {
    const CmnDff* dff = *p;
    s << "DFF#" << dff->id() << "(" << dff->name() << ")" << endl
      << "  Cell:   " << dff->cell()->cell()->name() << endl
      << "  Q:      " << dff->output1()->id_str() << endl
      << "  IQ:     " << dff->output2()->id_str() << endl
      << "  DATA(" << dff->input()->id_str() << ") = "
      << dff->input()->fanin(0)->id_str() << endl
      << "  CLOCK(" << dff->clock()->id_str() << ") = "
      << dff->clock()->fanin(0)->id_str() << endl;
    if ( dff->clear() ) {
      s << "  CLEAR(" << dff->clear()->id_str() << ") = "
	<< dff->clear()->fanin(0)->id_str() << endl;
    }
    if ( dff->preset() ) {
      s << "  PRESET(" << dff->preset()->id_str() << ") = "
	<< dff->preset()->fanin(0)->id_str() << endl;
    }
    s << endl;
  }

  const CmnLatchList& latch_list = network.latch_list();
  for (CmnLatchList::const_iterator p = latch_list.begin();
       p != latch_list.end(); ++ p) {
    const CmnLatch* latch = *p;
    s << "LATCH#" << latch->id() << "(" << latch->name() << ")" << endl
      << "  Cell:   " << latch->cell()->cell()->name() << endl
      << "  Q:      " << latch->output1()->id_str() << endl
      << "  IQ:     " << latch->output2()->id_str() << endl
      << "  DATA:   " << latch->input()->id_str() << endl;
    if ( latch->enable() ) {
      s << "  ENABLE: " << latch->enable()->id_str() << endl;
    }
    if ( latch->clear() ) {
      s << "  CLEAR: " << latch->clear()->id_str() << endl;
    }
    if ( latch->preset() ) {
      s << "  PRESET: " << latch->preset()->id_str() << endl;
    }
    s << endl;
  }

  const CmnNodeList& logic_list = network.logic_list();
  for (CmnNodeList::const_iterator p = logic_list.begin();
       p != logic_list.end(); ++ p) {
    const CmnNode* node = *p;
    const Cell* cell = node->cell();
    s << "CELL(" << node->id_str() << ") = "
      << cell->name() << "(";
    const char* comma = "";
    ymuint ni = node->ni();
    for (ymuint i = 0; i < ni; ++ i) {
      const CmnNode* inode = node->fanin(i);
      s << comma << inode->id_str();
      comma = ", ";
    }
    s << ")" << endl;
  }
}


BEGIN_NONAMESPACE

// ノード名を返す．
string
node_name(const CmnNode* node)
{
  return "n" + node->id_str();
}

END_NONAMESPACE


// 内容を Verilog-HDL 形式で s に出力する．
void
CmnDumper::dump_verilog(ostream& s,
			const CmnMgr& network)
{
  const CmnDffList& dff_list = network.dff_list();
  const CmnLatchList& latch_list = network.latch_list();
  const CmnNodeList& input_list = network.input_list();
  const CmnNodeList& output_list = network.output_list();
  const CmnNodeList& logic_list = network.logic_list();

  s << "module " << network.name() << "(";
  ymuint np = network.port_num();
  const char* sep = "";
  for (ymuint i = 0; i < np; ++ i) {
    const CmnPort* port = network.port(i);
    s << sep << "." << port->name() << "(";
    ymuint nb = port->bit_width();
    assert_cond( nb > 0, __FILE__, __LINE__);
    if ( nb == 1 ) {
      const CmnNode* input = port->input(0);
      s << node_name(input);
    }
    else {
      s << "{";
      const char* comma = "";
      for (ymuint j = 0; j < nb; ++ j) {
	ymuint idx = nb - j - 1;
	const CmnNode* input = port->input(idx);
	s << comma << node_name(input);
	comma = ", ";
      }
      s << "}";
    }
    s << ")";
    sep = ", ";
  }
  s << ");" << endl;

  for (CmnNodeList::const_iterator p = input_list.begin();
       p != input_list.end(); ++ p) {
    const CmnNode* node = *p;
    s << "  input  " << node_name(node) << ";" << endl;
  }

  for (CmnNodeList::const_iterator p = output_list.begin();
       p != output_list.end(); ++ p) {
    const CmnNode* node = *p;
    s << "  output " << node_name(node) << ";" << endl;
  }

  for (CmnDffList::const_iterator p = dff_list.begin();
       p != dff_list.end(); ++ p) {
  }

  for (CmnNodeList::const_iterator p = logic_list.begin();
       p != logic_list.end(); ++ p) {
    const CmnNode* node = *p;
    s << "  wire   " << node_name(node) << ";" << endl;
  }

  for (CmnNodeList::const_iterator p = output_list.begin();
       p != output_list.end(); ++ p) {
    const CmnNode* node = *p;
    const CmnNode* inode = node->fanin(0);
    assert_cond( inode != NULL, __FILE__, __LINE__);
    s << "  assign " << node_name(node)
      << " = " << node_name(inode) << ";" << endl;
  }

  for (CmnNodeList::const_iterator p = logic_list.begin();
       p != logic_list.end(); ++ p) {
    const CmnNode* node = *p;
    const Cell* cell = node->cell();
    assert_cond( cell != NULL, __FILE__, __LINE__);
    ymuint np = cell->pin_num();
    s << "  " << cell->name() << " U" << node->id() << " (";
    ymuint ipos = 0;
    const char* comma = "";
    for (ymuint i = 0; i < np; ++ i) {
      const CellPin* pin = cell->pin(i);
      const CmnNode* node1 = NULL;
      if ( pin->is_input() ) {
	node1 = node->fanin(ipos);
	++ ipos;
      }
      else if ( pin->is_output() ) {
	node1 = node;
      }
      s << comma << "." << pin->name() << "(" << node_name(node1) << ")";
      comma = ", ";
    }
    s << ");" << endl;
  }
  s << "endmodule" << endl;
}


END_NAMESPACE_YM_NETWORKS
