
/// @file libym_cell/dotlib/CellHandler.cc
/// @brief CellHandler の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "CellHandler.h"

#include "BusHandler.h"
#include "FfHandler.h"
#include "LatchHandler.h"
#include "PinHandler.h"
#include "StateTableHandler.h"
#include "LeakagePowerHandler.h"

#include "InternalPowerHandler.h"

#include "DummySimpleHandler.h"
#include "DummyComplexHandler.h"
#include "DummyGroupHandler.h"


BEGIN_NAMESPACE_YM_CELL_DOTLIB

//////////////////////////////////////////////////////////////////////
// クラス CellHandler
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] lib_handler 親のハンドラ
CellHandler::CellHandler(LibraryGroupHandler* lib_handler) :
  LibGroupHandler(lib_handler),
  mCell(NULL)
{
  DotLibHandler* dummy_simple = new DummySimpleHandler(parser());
  DotLibHandler* dummy_complex = new DummyComplexHandler(parser());
  DotLibHandler* dummy_group = new DummyGroupHandler(parser());

  // simple attributes
  reg_handler("area", dummy_simple);
  reg_handler("auxiliary_pad_cell", dummy_simple);
  reg_handler("base_name", dummy_simple);
  reg_handler("bus_naming_style", dummy_simple);
  reg_handler("cell_footprint", dummy_simple);
  reg_handler("cell_leakage_power", dummy_simple);
  reg_handler("clock_gating_integrated_cell", dummy_simple);
  reg_handler("contention_condition", dummy_simple);
  reg_handler("dont_fault", dummy_simple);
  reg_handler("dont_touch", dummy_simple);
  reg_handler("dont_use", dummy_simple);
  reg_handler("driver_type", dummy_simple);
  reg_handler("edif_name", dummy_simple);
  reg_handler("em_temp_degradation_factor", dummy_simple);
  reg_handler("fpga_domain_style", dummy_simple);
  reg_handler("geometry_print", dummy_simple);
  reg_handler("handle_negative_constraint", dummy_simple);
  reg_handler("io_type", dummy_simple);
  reg_handler("is_clock_gating_cell", dummy_simple);
  reg_handler("map_only", dummy_simple);
  reg_handler("pad_cell", dummy_simple);
  reg_handler("pad_type", dummy_simple);
  reg_handler("power_cell_type", dummy_simple);
  reg_handler("preferred", dummy_simple);
  reg_handler("scaling_factors", dummy_simple);
  reg_handler("single_bit_degenerate", dummy_simple);
  reg_handler("slew_type", dummy_simple);
  reg_handler("timing_model_type", dummy_simple);
  reg_handler("use_for_size_only", dummy_simple);
  reg_handler("vhdl_name", dummy_simple);

  reg_handler("is_filler_cell", dummy_simple);

  // complex attributes
  reg_handler("pin_opposite", dummy_complex);
  reg_handler("rail_connection", dummy_complex);
  reg_handler("power_supply_namestring", dummy_complex);
  reg_handler("resource_usage", dummy_complex);

  // group statements
  reg_handler("bus", new BusHandler(parser));
  reg_handler("dynamic_current", dummy_group);
  reg_handler("ff", new FfHandler(parser));
  reg_handler("ff_bank", dummy_group);
  reg_handler("functional_yield_metric", dummy_group);
  reg_handler("generated_clock", dummy_group);
  reg_handler("intrinsic_parasitic", dummy_group);
  reg_handler("latch", new LatchHandler(parser));
  reg_handler("latch_bank", dummy_group);
  reg_handler("leakage_current", dummy_group);
  reg_handler("leakage_power", new LeakagePowerHandler(parser));
  reg_handler("lut", dummy_group);
  reg_handler("mode_definition", dummy_group);
  reg_handler("pin", new PinHandler(parser));
  reg_handler("routing_track", dummy_group);
  reg_handler("statetable", new StateTableHandler(parser));

  reg_handler("internal_power", new CellInternalPowerHandler(parser));
}

// @brief デストラクタ
CellHandler::~CellHandler()
{
}

// @brief グループ名を読み込んだ時の処理
// @param[in] attr_name 属性名
// @param[in] token_list トークンのリスト
bool
CellHandler::begin_group(const string& attr_name,
			 const vector<Token>& token_list)
{
  assert_cond( attr_name == "cell", __FILE__, __LINE__);

  if ( token_list.size() != 1 ) {
    FileRegion loc;
    if ( token_list.size() > 1 ) {
      loc = token_list[2].loc();
    }
    msg_mgr().put_msg(__FILE__, __LINE__, loc,
		      kMsgError,
		      "DOTLIB_PARSER",
		      "cell statement require only one token");
    return false;
  }

  if ( token_list[0].type() != STR ) {
    msg_mgr().put_msg(__FILE__, __LINE__, token_list[0].loc(),
		      kMsgError,
		      "DOTLIB_PARSER",
		      "string type is expected.");
    return false;
  }

  string name = token_list[0].value();
  mCell =
  return true;
}

// @brief グループ内のステートメントをすべて処理したときに呼ばれる関数
bool
CellHandler::end_group()
{
  return true;
}

// @brief 対象のセルを返す．
CiCell*
CellHandler::cell()
{
  return mCell;
}


END_NAMESPACE_YM_CELL_DOTLIB
