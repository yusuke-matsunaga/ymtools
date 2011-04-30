
/// @file libym_dotlib/tests/ptview/ptview_main.cc
/// @brief ptview の main ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: vlview_main.cc 2507 2009-10-17 16:24:02Z matsunaga $
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include <QApplication>
#include <QSplitter>
#include "VerilogView.h"
#include "ParseTreeModel.h"
#include "ParseTreeView.h"

#include "ym_dotlib/DotlibParser.h"


int
main(int argc,
     char** argv)
{
  using namespace nsYm;
  using namespace nsYm::nsDotlib;

  QApplication app(argc, argv);

  MsgMgr msg_mgr;
  DotlibParser parser(msg_mgr);

  // パース木を作る．
  const DotlibNode* library = parser.read_file(argv[1], false);
  if ( library == NULL ) {
    return 1;
  }

  ParseTreeModel* pt_model = new ParseTreeModel;
  pt_model->set_pt(library);

  // 表示用ウィジェットを作る．
  VerilogView* vlview = new VerilogView;
  ParseTreeView* pt_view = new ParseTreeView;

  QSplitter* splitter = new QSplitter(Qt::Horizontal);
  splitter->addWidget(pt_view);
  splitter->addWidget(vlview);
  splitter->setStretchFactor(0, 3);
  splitter->setStretchFactor(1, 4);
  splitter->resize(1024, 760);

  if ( !vlview->open(argv[1]) ) {
    return 2;
  }
  pt_view->setModel(pt_model);

  QObject::connect(pt_view, SIGNAL(select_token(int, int, int, int)),
		   vlview, SLOT(hilight(int, int, int, int)));

  splitter->show();

  return app.exec();
}