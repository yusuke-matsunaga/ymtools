﻿
/// @file iscas89test.cc
/// @brief iscas89 ファイルの読み書きのテスト
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2013-2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmNetworks/TgNetwork.h"
#include "YmNetworks/TgIscas89Reader.h"
#include "YmUtils/MsgMgr.h"
#include "YmUtils/MsgHandler.h"


int
main(int argc,
     char** argv)
{
  using namespace std;
  using namespace nsYm;

  if ( argc != 2 ) {
    cerr << "USAGE : " << argv[0] << " iscas89-file" << endl;
    return 2;
  }
  string filename = argv[1];

  try {
    MsgHandler* msg_handler = new StreamMsgHandler(&cerr);
    TgIscas89Reader read;

    MsgMgr::reg_handler(msg_handler);

    TgNetwork network;

    if ( !read(filename, network) ) {
      cerr << "Error in reading " << filename << endl;
      return 4;
    }

    dump(cout, network);

  }
  catch ( AssertError x) {
    cout << x << endl;
  }

  return 0;
}
