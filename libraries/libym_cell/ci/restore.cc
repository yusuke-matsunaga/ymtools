
/// @file libym_cell/ci/restore.cc
/// @brief バイナリファイルを読み込む関数
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010 Yusuke Matsunaga
/// All rights reserved.


#include "CiLibrary.h"
#include "CiPin.h"
#include "ym_utils/BinIO.h"


BEGIN_NAMESPACE_YM_CELL

const CellLibrary*
restore_library(istream& s)
{
  string name = BinIO::read_str(s);

  CiLibrary* library = new CiLibrary(name);

  ymuint nc = BinIO::read_32(s);
  library->set_cell_num(nc);

  for (ymuint i = 0; i < nc; ++ i) {
    ShString name( BinIO::read_str(s) );
    CellArea area( BinIO::read_double(s) );
    ymuint np = BinIO::read_32(s);
    ymuint nb = BinIO::read_32(s);
    ymuint nc = BinIO::read_32(s);
    CiCell* cell = library->new_cell(i, name, area, np, nb, nc);

    ymuint nt = BinIO::read_32(s);
    vector<const CellTiming*> timing_list(nt);
    for (ymuint j = 0; j < nt; ++ j) {
      CellTime i_r( BinIO::read_double(s) );
      CellTime i_f( BinIO::read_double(s) );
      CellTime s_r( BinIO::read_double(s) );
      CellTime s_f( BinIO::read_double(s) );
      CellResistance r_r( BinIO::read_double(s) );
      CellResistance f_r( BinIO::read_double(s) );
      timing_list[j] = library->new_timing(j, kTimingCombinational,
					   i_r, i_f, s_r, s_f, r_r, f_r);
    }

    for (ymuint j = 0; j < np; ++ j) {
      ShString name( BinIO::read_str(s) );
      ymuint d = BinIO::read_32(s);
      switch ( d ) {
      case 1: // 入力
	{
	  CellCapacitance cap( BinIO::read_double(s) );
	  CellCapacitance r_cap( BinIO::read_double(s) );
	  CellCapacitance f_cap( BinIO::read_double(s) );
	  library->new_cell_input(cell, j, name, cap, r_cap, f_cap);
	}
	break;

      case 2: // 出力
	{
	  CellCapacitance max_f( BinIO::read_double(s) );
	  CellCapacitance min_f( BinIO::read_double(s) );
	  CellCapacitance max_c( BinIO::read_double(s) );
	  CellCapacitance min_c( BinIO::read_double(s) );
	  CellTime max_t( BinIO::read_double(s) );
	  CellTime min_t( BinIO::read_double(s) );
	  CiOutputPin* pin = library->new_cell_output(cell, j, name,
						      max_f, min_f,
						      max_c, min_c,
						      max_t, min_t);
	  for ( ; ; ) {
	    ymuint unate = BinIO::read_32(s);
	    if ( unate == 0 ) break;
	    if ( unate == 1 ) {
	      ymuint pin_id = BinIO::read_32(s);
	      ymuint timing_id = BinIO::read_32(s);
	      const CellTiming* timing = timing_list[timing_id];
	      library->set_opin_timing(pin, pin_id, kSensePosiUnate, timing);
	    }
	    else if ( unate == 2 ) {
	      ymuint pin_id = BinIO::read_32(s);
	      ymuint timing_id = BinIO::read_32(s);
	      const CellTiming* timing = timing_list[timing_id];
	      library->set_opin_timing(pin, pin_id, kSenseNegaUnate, timing);
	    }
	  }
	}
	break;

      case 3: // 入出力
	{
	  CellCapacitance cap( BinIO::read_double(s) );
	  CellCapacitance r_cap( BinIO::read_double(s) );
	  CellCapacitance f_cap( BinIO::read_double(s) );
	  CellCapacitance max_f( BinIO::read_double(s) );
	  CellCapacitance min_f( BinIO::read_double(s) );
	  CellCapacitance max_c( BinIO::read_double(s) );
	  CellCapacitance min_c( BinIO::read_double(s) );
	  CellTime max_t( BinIO::read_double(s) );
	  CellTime min_t( BinIO::read_double(s) );
	  CiInoutPin* pin = library->new_cell_inout(cell, j, name,
						    cap, r_cap, f_cap,
						    max_f, min_f,
						    max_c, min_c,
						    max_t, min_t);
	  for ( ; ; ) {
	    ymuint unate = BinIO::read_32(s);
	    if ( unate == 0 ) break;
	    if ( unate == 1 ) {
	      ymuint pin_id = BinIO::read_32(s);
	      ymuint timing_id = BinIO::read_32(s);
	      const CellTiming* timing = timing_list[timing_id];
	      library->set_opin_timing(pin, pin_id, kSensePosiUnate, timing);
	    }
	    else if ( unate == 2 ) {
	      ymuint pin_id = BinIO::read_32(s);
	      ymuint timing_id = BinIO::read_32(s);
	      const CellTiming* timing = timing_list[timing_id];
	      library->set_opin_timing(pin, pin_id, kSenseNegaUnate, timing);
	    }
	  }
	}
	break;
      }
    }
  }

  return library;
}

END_NAMESPACE_YM_CELL