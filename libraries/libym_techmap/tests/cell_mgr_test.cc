
/// @file libym_techmap/test/cell_mgr_test.cc
/// @brief CellMgr のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "cellmap/CellMgr.h"
#include "cellmap/patgen/PgFuncMgr.h"
#include "ym_cell/CellMislibReader.h"
#include "ym_cell/CellLibrary.h"


BEGIN_NAMESPACE_YM_CELLMAP_PATGEN

// セルライブラリのパタンを生成してダンプする．
void
dump_patdata(ostream& s,
	     const CellLibrary& library)
{
  PgFuncMgr pgf_mgr;
  pgf_mgr.set_library(&library);

  pg_dump(s, pgf_mgr);
}

END_NAMESPACE_YM_CELLMAP_PATGEN


BEGIN_NAMESPACE_YM_CELLMAP

bool
dump_load_test(const char* in_filename,
	       const char* data_filename)
{
  using nsPatgen::dump_patdata;

  CellMislibReader reader;
  const CellLibrary* library = reader.read(in_filename);
  if ( library == NULL ) {
    cerr << in_filename << ": Error in reading library" << endl;
    return false;
  }

  ofstream os;
  os.open(data_filename, ios::binary);
  if ( !os ) {
    // エラー
    cerr << "Could not create " << data_filename << endl;
    return false;
  }

  dump_patdata(os, *library);

  os.close();

  CellMgr cell_mgr;
  {
    ifstream ifs;
    ifs.open(data_filename, ios::binary);
    if ( !ifs ) {
      // エラー
      cerr << "Could not open " << data_filename << endl;
      return false;
    }
    cell_mgr.load_library(ifs);
  }
  dump(cout, cell_mgr);

  return true;
}

END_NAMESPACE_YM_CELLMAP


int
main(int argc,
     char** argv)
{
  using nsYm::nsCellmap::dump_load_test;

  const char* filename = argv[1];
  const char* datafile = "patdata.bin";

  if ( !dump_load_test(filename, datafile) ) {
    return -1;
  }

  return 0;
}
