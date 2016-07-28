
/// @file bliftest.cc
/// @brief blif ファイルの読み書きのテスト
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym_networks/BNetwork.h"
#include "ym_networks/BlifBNetConv.h"
#include "ym_networks/BNetBlifWriter.h"
#include "ym_blif/BlifNetwork.h"
#include "ym_blif/BlifNetworkReader.h"
#include "ym_utils/MsgMgr.h"
#include "ym_utils/MsgHandler.h"


int
main(int argc,
     char** argv)
{
  using namespace std;
  using namespace nsYm;

  if ( argc != 2 ) {
    cerr << "USAGE : " << argv[0] << " blif-file" << endl;
    return 2;
  }
  string filename = argv[1];

  try {
    MsgHandler* msg_handler = new StreamMsgHandler(&cerr);
    BlifNetworkReader reader;

    MsgMgr::reg_handler(msg_handler);

    BlifNetwork blif_network;

    if ( !reader.read(filename, blif_network) ) {
      cerr << "Error in reading " << filename << endl;
      return 4;
    }

    BNetwork network;

    BlifBNetConv conv;
    if ( !conv(blif_network, network) ) {
      cerr << "Error in converting BlifNetwork to BNetwork" << endl;
      return 5;
    }

    BNetwork network2 = network;

    BNetBlifWriter writer;
    writer.dump(cout, network2);
  }
  catch ( AssertError x) {
    cout << x << endl;
  }

  return 0;
}