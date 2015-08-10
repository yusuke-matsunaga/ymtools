﻿
/// @file mislib_parser_test.cc
/// @brief MislibParser のテスト用プログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "mislib/MislibParser.h"
#include "mislib/MislibMgr.h"
#include "mislib/MislibNode.h"

#include "YmUtils/MsgMgr.h"
#include "YmUtils/MsgHandler.h"


BEGIN_NAMESPACE_YM_MISLIB

int
mislib_parser_test(const string& filename)
{
  MislibMgr mgr;
  MislibParser parser;

  MsgHandler* mh = new StreamMsgHandler(&cerr);
  mh->set_mask(kMaskAll);
  mh->delete_mask(kMsgInfo);
  mh->delete_mask(kMsgDebug);
  MsgMgr::reg_handler(mh);

  if ( !parser.read_file(filename, mgr) ) {
    return -1;
  }

  const MislibNode* gate_list = mgr.gate_list();
  for (const MislibNode* gate = gate_list->top();
       gate != nullptr; gate = gate->next()) {
    gate->dump(cout);
  }

  return 0;
}

END_NAMESPACE_YM_MISLIB


int
main(int argc,
     const char** argv)
{
  using namespace std;

  if ( argc != 2 ) {
    cerr << "USAGE: " << argv[0] << " <filename>" << endl;
    return 255;
  }

  int stat = nsYm::nsCell::nsMislib::mislib_parser_test(argv[1]);

  return stat;
}
