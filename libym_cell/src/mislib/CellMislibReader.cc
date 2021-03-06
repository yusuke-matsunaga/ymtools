﻿
/// @file CellMislibReader.cc
/// @brief CellMislibReader の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmCell/CellMislibReader.h"

#include "YmCell/CellLibrary.h"
#include "YmCell/CellArea.h"
#include "YmCell/CellCapacitance.h"
#include "YmCell/CellResistance.h"
#include "YmCell/CellTime.h"
#include "YmCell/CellTiming.h"

#include "MislibParser.h"
#include "MislibMgr.h"
#include "MislibNode.h"

#include "YmLogic/Expr.h"
#include "YmLogic/TvFunc.h"
#include "YmUtils/MsgMgr.h"


BEGIN_NAMESPACE_YM_MISLIB

BEGIN_NONAMESPACE

typedef HashMap<ShString, ymuint> NameMap;

// 論理式を表すパース木をスキャンして端子名のリストを作る．
// param[in] node パース木の根のノード
// param[out] name_list 名前が現れた順に格納されたリスト
// param[out] name_map 名前をキーにして name_list 中の位置を返す連想配列
void
dfs(const MislibNode* node,
    vector<ShString>& name_list,
    NameMap& name_map)
{
  switch ( node->type() ) {
  case MislibNode::kConst0:
  case MislibNode::kConst1:
    return;

  case MislibNode::kStr:
    {
      ShString name = node->str();
      if ( !name_map.check(name) ) {
	// 登録する．
	name_map.add(name, name_list.size());
	name_list.push_back(name);
      }
    }
    break;

  case MislibNode::kNot:
    dfs(node->child1(), name_list, name_map);
    break;

  case MislibNode::kAnd:
  case MislibNode::kOr:
  case MislibNode::kXor:
    dfs(node->child1(), name_list, name_map);
    dfs(node->child2(), name_list, name_map);
    break;

  default:
    ASSERT_NOT_REACHED;
  }
}

// @brief MislibNode から CellLibrary を生成する．
// @param[in] lib_name ライブラリ名
// @param[in] gate_list パース木のルート
// @return 生成したライブラリを返す．
// @note 生成が失敗したら nullptr を返す．
const CellLibrary*
gen_library(const string& lib_name,
	    const MislibNode* gate_list)
{
  // ライブラリの生成
  CellLibrary* library = CellLibrary::new_obj();
  library->set_name(lib_name);

  // セル数の設定
  ymuint cell_num = 0;
  for (const MislibNode* gate = gate_list->top(); gate; gate = gate->next()) {
    ++ cell_num;
  }
  library->set_cell_num(cell_num);

  // セルの内容の設定
  ymuint cell_id = 0;
  for (const MislibNode* gate = gate_list->top(); gate;
       gate = gate->next(), ++ cell_id) {
    ShString name = gate->name()->str();
    CellArea area(gate->area()->num());
    ShString opin_name = gate->opin_name()->str();
    const MislibNode* opin_expr = gate->opin_expr();
    const MislibNode* ipin_top = gate->ipin_top();
    vector<const MislibNode*> ipin_array;
    vector<ShString> ipin_name_list;
    NameMap ipin_name_map;
    bool wildcard_pin = false;
    if ( ipin_top != nullptr ) {
      if ( ipin_top->name() != nullptr ) {
	// 通常の入力ピン定義がある場合
	// ipin_list の順に入力ピンを作る．
	for (const MislibNode* pin = ipin_top; pin; pin = pin->next()) {
	  ASSERT_COND( pin->type() == MislibNode::kPin );
	  ShString name = pin->name()->str();
	  ASSERT_COND( !ipin_name_map.check(name) );
	  ipin_name_map.add(name, ipin_array.size());
	  ipin_array.push_back(pin);
	  ipin_name_list.push_back(name);
	}
      }
      else {
	// ワイルドカードの場合
	// 論理式に現れる順に入力ピンを作る．
	wildcard_pin = true;
	dfs(opin_expr, ipin_name_list, ipin_name_map);
	for (ymuint i = 0; i < ipin_name_list.size(); ++ i) {
	  ipin_array.push_back(ipin_top);
	}
      }
    }

    ymuint ni = ipin_name_list.size();
    Expr function = opin_expr->to_expr(ipin_name_map);
    vector<bool> output_array(1, true);
    vector<Expr> logic_array(1, function);
    vector<Expr> tristate_array(1, Expr::make_zero());
    library->new_logic_cell(cell_id, name, area,
			    ni, 1, 0, 0, 0,
			    output_array,
			    logic_array,
			    tristate_array);
    for (ymuint i = 0; i < ni; ++ i) {
      // 入力ピンの設定
      ShString name = ipin_name_list[i];
      const MislibNode* pin = ipin_array[i];
      CellCapacitance load(pin->input_load()->num());
      library->new_cell_input(cell_id, i, i, name, load, load, load);
    }
    // 出力ピンの設定
    library->new_cell_output(cell_id, ni, 0, opin_name,
			     true, function,
			     Expr::make_zero(),
			     CellCapacitance::infty(),
			     CellCapacitance(0.0),
			     CellCapacitance::infty(),
			     CellCapacitance(0.0),
			     CellTime::infty(),
			     CellTime(0.0));

    // タイミング情報の生成
    vector<ymuint> tid_array(ni);
    if ( !wildcard_pin ) {
      library->set_timing_num(cell_id, ni);
      for (ymuint i = 0; i < ni; ++ i) {
	const MislibNode* pt_pin = ipin_array[i];
	CellTime r_i(pt_pin->rise_block_delay()->num());
	CellResistance r_r(pt_pin->rise_fanout_delay()->num());
	CellTime f_i(pt_pin->fall_block_delay()->num());
	CellResistance f_r(pt_pin->fall_fanout_delay()->num());
	library->new_timing_generic(cell_id, i,
				    kCellTimingCombinational,
				    Expr::make_one(),
				    r_i, f_i,
				    CellTime(0.0), CellTime(0.0),
				    r_r, f_r);
	tid_array[i] = i;
      }
    }
    else { // ipin_list->type() == MislibNode::kPin
      library->set_timing_num(cell_id, 1);
      const MislibNode* pt_pin = ipin_top;
      CellTime r_i(pt_pin->rise_block_delay()->num());
      CellResistance r_r(pt_pin->rise_fanout_delay()->num());
      CellTime f_i(pt_pin->fall_block_delay()->num());
      CellResistance f_r(pt_pin->fall_fanout_delay()->num());
      library->new_timing_generic(cell_id, 0,
				  kCellTimingCombinational,
				  Expr::make_one(),
				  r_i, f_i,
				  CellTime(0.0), CellTime(0.0),
				  r_r, f_r);
      for (ymuint i = 0; i < ni; ++ i) {
	tid_array[i] = 0;
      }
    }

    TvFunc tv_function = function.make_tv(ni);
    for (ymuint i = 0; i < ni; ++ i) {
      // タイミング情報の設定
      VarId var(i);
      const MislibNode* pt_pin = ipin_array[i];
      TvFunc p_func = tv_function.cofactor(var, false);
      TvFunc n_func = tv_function.cofactor(var, true);
      tCellTimingSense sense_real = kCellNonUnate;
      bool redundant = false;
      if ( ~p_func && n_func ) {
	if ( ~n_func && p_func ) {
	  sense_real = kCellNonUnate;
	}
	else {
	  sense_real = kCellNegaUnate;
	}
      }
      else {
	if ( ~n_func && p_func ) {
	  sense_real = kCellPosiUnate;
	}
	else {
	  // つまり p_func == n_func ということ．
	  // つまりこの変数は出力に影響しない．
	  ostringstream buf;
	  buf << "The output function does not depend on the input pin, "
	      << pt_pin->name()->str() << ".";
	  MsgMgr::put_msg(__FILE__, __LINE__,
			  pt_pin->loc(),
			  kMsgWarning,
			  "MISLIB_PARSER",
			  buf.str());
	  redundant = true;
	}
      }

      if ( redundant ) {
	continue;
      }

      tCellTimingSense sense = kCellNonUnate;
      switch ( pt_pin->phase()->type() ) {
      case MislibNode::kNoninv:
	sense = kCellPosiUnate;
	break;

      case MislibNode::kInv:
	sense = kCellNegaUnate;
	break;

      case MislibNode::kUnknown:
	sense = kCellNonUnate;
	break;

      default:
	ASSERT_NOT_REACHED; break;
      }

      if ( sense != sense_real ) {
	ostringstream buf;
	buf << "Phase description does not match the logic expression. "
	    << "Ignored.";
	MsgMgr::put_msg(__FILE__, __LINE__,
			pt_pin->phase()->loc(),
			kMsgWarning,
			"MISLIB_PARSER",
			buf.str());
	sense = sense_real;
      }
      if ( sense == kCellNonUnate ) {
	library->set_timing(cell_id, i, 0, kCellPosiUnate,
			    vector<ymuint>(1, tid_array[i]));
	library->set_timing(cell_id, i, 0, kCellNegaUnate,
			    vector<ymuint>(1, tid_array[i]));
      }
      else {
	library->set_timing(cell_id, i, 0, sense,
			    vector<ymuint>(1, tid_array[i]));
      }
    }
  }

  library->compile();

  return library;
}

END_NONAMESPACE

END_NAMESPACE_YM_MISLIB


BEGIN_NAMESPACE_YM_CELL

//////////////////////////////////////////////////////////////////////
// Mislibファイルを読んで CellLibrary を作るクラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
CellMislibReader::CellMislibReader()
{
}

// @brief デストラクタ
CellMislibReader::~CellMislibReader()
{
}

// @brief mislib ファイルを読み込む
// @param[in] filename ファイル名
// @return 読み込んで作成したセルライブラリを返す．
// @note エラーが起きたら nullptr を返す．
const CellLibrary*
CellMislibReader::operator()(const string& filename)
{
  using namespace nsMislib;

  MislibMgr mgr;
  MislibParser parser;
  if ( !parser.read_file(filename, mgr) ) {
    return nullptr;
  }
  return gen_library(filename, mgr.gate_list());
}

// @brief mislib ファイルを読み込む
// @param[in] filename ファイル名
// @return 読み込んで作成したセルライブラリを返す．
// @note エラーが起きたら nullptr を返す．
const CellLibrary*
CellMislibReader::operator()(const char* filename)
{
  using namespace nsMislib;

  MislibMgr mgr;
  MislibParser parser;
  if ( !parser.read_file(filename, mgr) ) {
    return nullptr;
  }
  return gen_library(filename, mgr.gate_list());
}

END_NAMESPACE_YM_CELL
