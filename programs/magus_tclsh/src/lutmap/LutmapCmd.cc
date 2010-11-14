
/// @file magus/lutmap/LutmapCmd.cc
/// @brief LutmapCmd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: LutmapCmd.cc 2507 2009-10-17 16:24:02Z matsunaga $
///
/// Copyright (C) 2005-2010 Yusuke Matsunaga
/// All rights reserved.


#include "LutmapCmd.h"
#include "ym_lutmap/LnGraph.h"


BEGIN_NAMESPACE_MAGUS

//////////////////////////////////////////////////////////////////////
// LUT mapping の基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
LutmapCmd::LutmapCmd(NetMgr* mgr,
		     LutmapData* data) :
  MagBNetCmd(mgr),
  mData(data)
{
}

// @brief デストラクタ
LutmapCmd::~LutmapCmd()
{
}

// @brief SbjGraph を得る．
SbjGraph&
LutmapCmd::sbjgraph()
{
  return mData->mSbjGraph;
}

// @brief LutNetwork を得る．
LnGraph&
LutmapCmd::lutnetwork()
{
  return mData->mLutNetwork;
}


int
lutmap_init(Tcl_Interp* interp,
	    NetMgr* mgr)
{
  LutmapData* data = new LutmapData;

  TclCmdBinder2<Conv2SbjCmd, NetMgr*, LutmapData*>::reg(interp, mgr, data,
							"magus::lutmap::conv2sbj");
  TclCmdBinder2<ReadVerilogCmd, NetMgr*, LutmapData*>::reg(interp, mgr, data,
							   "magus::lutmap::read_verilog");
  TclCmdBinder2<DumpSbjCmd, NetMgr*, LutmapData*>::reg(interp, mgr, data,
						       "magus::lutmap::dump_sbjgraph");
  TclCmdBinder2<AreaMapCmd, NetMgr*, LutmapData*>::reg(interp, mgr, data,
						       "magus::lutmap::area_map");
  TclCmdBinder2<DelayMapCmd, NetMgr*, LutmapData*>::reg(interp, mgr, data,
							"magus::lutmap::delay_map");
  TclCmdBinder2<DumpLutCmd, NetMgr*, LutmapData*>::reg(interp, mgr, data,
						       "magus::lutmap::dump_lutnetwork");
  TclCmdBinder2<Conv2BNetCmd, NetMgr*, LutmapData*>::reg(interp, mgr, data,
							 "magus::lutmap::conv2bnet");
  TclCmdBinder2<WriteVqmCmd, NetMgr*, LutmapData*>::reg(interp, mgr, data,
							"magus::lutmap::write_vqm");


  const char* init =
    "namespace eval tclreadline {\n"
    "namespace eval magus {\n"
    "namespace eval lutmap {\n"
    "proc complete(conv2sbj) { text start end line pos mod } { return \"\" }\n"
    "proc complete(read_verilog) { text start end line pos mod } { return \"\" }\n"
    "proc complete(dump_sbjgraph) { text start end line pos mod } { return \"\" }\n"
    "proc complete(area_map) { text start end line pos mod } { return \"\" }\n"
    "proc complete(delay_map) { text start end line pos mod } { return \"\" }\n"
    "proc complete(dump_lutnetwork) { text start end line pos mod } { return \"\" }\n"
    "proc complete(conv2bnet) { text start end line pos mod } { return \"\" }\n"
    "proc complete(write_vqn) { text start end line pos mod } { return \"\" }\n"
    "}\n"
    "}\n"
    "}\n";

  if ( Tcl_Eval(interp, (char*) init) == TCL_ERROR ) {
    return TCL_ERROR;
  }

  return TCL_OK;
}

END_NAMESPACE_MAGUS
