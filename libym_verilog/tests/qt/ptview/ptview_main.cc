﻿
/// @file ptview_main.cc
/// @brief ptview の main ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014 Yusuke Matsunaga
/// All rights reserved.


#include <QApplication>
#include <QSplitter>
#include "VerilogView.h"
#include "TokenView.h"
#include "TokenListModel.h"
#include "ParseTreeModel.h"
#include "ParseTreeView.h"
#include "GoToLine.h"

#include "Lex.h"
#include "YmVerilog/VlMgr.h"

#include "YmUtils/MsgMgr.h"
#include "YmUtils/MsgHandler.h"


int
main(int argc,
     char** argv)
{
  using namespace nsYm;
  using namespace nsYm::nsVerilog;

  QApplication app(argc, argv);

  MsgHandler* tmh = new StreamMsgHandler(&cerr);
  MsgMgr::reg_handler(tmh);

  VlMgr vl_mgr;
  Lex lex;

  // サーチパスリスト
  // 今は未設定
  const char* spath = nullptr;
  SearchPathList splist;
  if ( spath ) {
    splist = SearchPathList(spath);
  }
  // トークンリストを作る．
  lex.set_searchpath(splist);
  if ( !lex.open_file(argv[1]) ) {
    cerr << argv[1] << " : could not open" << endl;
    return 1;
  }

  // トークンを読み出して token_model にセットする．
  TokenListModel* token_model = new TokenListModel;
  for ( ; ; ) {
    int id = lex.get_token();
    if ( id == EOF ) {
      break;
    }
    token_model->add_token(id, lex.cur_token_loc(), lex.cur_string());
  }

  // パース木を作る．
  if ( !vl_mgr.read_file(argv[1], splist) ) {
    return 1;
  }
  ParseTreeModel* pt_model = new ParseTreeModel;
  pt_model->set_pt(vl_mgr);

  // 表示用ウィジェットを作る．
  VerilogView* vlview = new VerilogView;
  TokenView* token_view = new TokenView;
  ParseTreeView* pt_view = new ParseTreeView;

  QSplitter* splitter = new QSplitter(Qt::Horizontal);
  splitter->addWidget(pt_view);
  splitter->addWidget(vlview);
  splitter->addWidget(token_view);
  splitter->setStretchFactor(0, 3);
  splitter->setStretchFactor(1, 4);
  splitter->setStretchFactor(2, 2);
  splitter->resize(1024, 760);

  token_view->setModel(token_model);

  if ( !vlview->open(argv[1]) ) {
    return 2;
  }
  pt_view->setModel(pt_model);

  QObject::connect(token_view, SIGNAL(select_token(int, int, int, int)),
		   vlview, SLOT(hilight(int, int, int, int)));

  QObject::connect(pt_view, SIGNAL(select_token(int, int, int, int)),
		   vlview, SLOT(hilight(int, int, int, int)));

  splitter->show();

#if 0
  GoToLine* goto_line = new GoToLine;
  QObject::connect(goto_line, SIGNAL(on_goto(int)),
		   vlview, SLOT(hilight(int)));

  goto_line->set_minimum(1);
  goto_line->set_maximum(100000);

  goto_line->show();
#endif

  return app.exec();
}
