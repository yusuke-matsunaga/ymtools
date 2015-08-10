﻿
/// @file cell_mgr_test.cc
/// @brief CellMgr のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmCell/CellLibrary.h"
#include "YmCell/CellMislibReader.h"
#include "YmCell/CellDotlibReader.h"
#include "YmCell/Cell.h"
#include "YmCell/CellPin.h"


int
main(int argc,
     char** argv)
{
  using namespace std;
  using namespace nsYm::nsCell;

  if ( argc < 2 ) {
    cerr << "Usage: " << argv[0] << " [--liberty] <liberty-file>" << endl;
    return 1;
  }
  ymuint base = 1;
  bool dotlib = false;
  if ( argc == 3 && strcmp(argv[1], "--liberty") == 0 ) {
    dotlib = true;
    base = 2;
  }

  const char* filename = argv[base];

  const CellLibrary* library = nullptr;
  if ( dotlib ) {
    CellDotlibReader dotlib_reader;
    library = dotlib_reader(filename);
  }
  else {
    CellMislibReader mislib_reader;
    library = mislib_reader(filename);
  }
  if ( library == nullptr ) {
    cerr << filename << ": Error in reading library" << endl;
    return 1;
  }

  display_library(cout, *library);

  ymuint n = library->cell_num();
  for (ymuint i = 0; i < n; ++ i) {
    const Cell* cell = library->cell(i);
    cout << "Cell: " << cell->name() << endl;
    const Cell* cell2 = library->cell(cell->name());
    if ( cell != cell2 ) {
      cout << "Error" << endl;
    }

    ymuint np = cell->pin_num();
    for (ymuint j = 0; j < np; ++ j) {
      const CellPin* pin = cell->pin(j);
      cout << "  Pin: " << pin->name() << endl;
      const CellPin* pin2 = cell->pin(pin->name());
      if ( pin != pin2 ) {
	cout << "  Error" << endl;
      }
    }
  }

  return 0;
}
