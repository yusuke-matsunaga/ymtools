﻿
/// @file CiLibrary.cc
/// @brief CiLibrary の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "CiLibrary.h"
#include "CiClass.h"
#include "CiGroup.h"
#include "CiCell.h"
#include "CiLogicCell.h"
#include "CiFFCell.h"
#include "CiLatchCell.h"
#include "CiFsmCell.h"
#include "CiPin.h"
#include "CiTiming.h"
#include "CiLutTemplate.h"
#include "CiLut.h"

#include "LibComp.h"
#include "LcGroupMgr.h"
#include "LcClass.h"
#include "LcGroup.h"
#include "LcPatMgr.h"


BEGIN_NAMESPACE_YM_CELL

using namespace nsLibcomp;

//////////////////////////////////////////////////////////////////////
// クラス CellLibrary
//////////////////////////////////////////////////////////////////////

// @brief 実際のオブジェクトを作るクラスメソッド
CellLibrary*
CellLibrary::new_obj()
{
  return new CiLibrary();
}


//////////////////////////////////////////////////////////////////////
// クラス CiLibrary
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
CiLibrary::CiLibrary() :
  mAlloc(4096),
  mLutHash(mAlloc),
  mCellHash(mAlloc)
{
  mTechnology = kCellTechCmos;
  mDelayModel = kCellDelayGenericCmos;
  mLutTemplateNum = 0;
  mLutTemplateArray = nullptr;
  mCellNum = 0;
  mCellArray = nullptr;
  mGroupNum = 0;
  mGroupArray = nullptr;
  mClassNum = 0;
  mClassArray = nullptr;
}

// @brief デストラクタ
CiLibrary::~CiLibrary()
{
}

// @brief 名前の取得
string
CiLibrary::name() const
{
  return mName;
}

// @brief テクノロジの取得
tCellTechnology
CiLibrary::technology() const
{
  return mTechnology;
}

// @brief 遅延モデルの取得
// 返り値は
// - kCellDelayGenericCmos
// - kCellDelayTableLookup
// - kCellDelayPiecewiseCmos
// - kCellDelayCmos2
// - kCellDelayDcm
// のいずれか
tCellDelayModel
CiLibrary::delay_model() const
{
  return mDelayModel;
}

// @brief バス命名規則の取得
string
CiLibrary::bus_naming_style() const
{
  return mBusNamingStyle;
}

// @brief 日付情報の取得
string
CiLibrary::date() const
{
  return mDate;
}

/// @brief リビジョン情報の取得
string
CiLibrary::revision() const
{
  return mRevision;
}

// @brief コメント情報の取得
string
CiLibrary::comment() const
{
  return mComment;
}

// @brief 時間単位の取得
string
CiLibrary::time_unit() const
{
  return mTimeUnit;
}

// @brief 電圧単位の取得
string
CiLibrary::voltage_unit() const
{
  return mVoltageUnit;
}

// @brief 電流単位の取得
string
CiLibrary::current_unit() const
{
  return mCurrentUnit;
}

// @brief 抵抗単位の取得
string
CiLibrary::pulling_resistance_unit() const
{
  return mPullingResistanceUnit;
}

// @brief 容量単位の取得
// @note なぜかここだけインターフェイスが異なる．
double
CiLibrary::capacitive_load_unit() const
{
  return mCapacitiveLoadUnit;
}

// @brief 容量単位文字列の取得
// @note なぜかここだけインターフェイスが異なる．
string
CiLibrary::capacitive_load_unit_str() const
{
  return mCapacitiveLoadUnitStr;
}

// @brief 電力単位の取得
string
CiLibrary::leakage_power_unit() const
{
  return mLeakagePowerUnit;
}

// @brief 遅延テーブルのテンプレート数の取得
ymuint
CiLibrary::lu_table_template_num() const
{
  return mLutTemplateNum;
}

// @brief 遅延テーブルのテンプレートの取得
// @param[in] pos 位置番号 ( 0 <= pos < lu_table_template_num() )
const CellLutTemplate*
CiLibrary::lu_table_template(ymuint pos) const
{
  ASSERT_COND( pos < lu_table_template_num() );
  return mLutTemplateArray[pos];
}

// @brief ルックアップテーブルのテンプレートの取得
// @param[in] name テンプレート名
// @note なければ nullptr を返す．
const CellLutTemplate*
CiLibrary::lu_table_template(const char* name) const
{
  return mLutHash.get(ShString(name));
}

// @brief バスタイプの取得
// @param[in] name バスタイプ名
// @note なければ nullptr を返す．
const CellBusType*
CiLibrary::bus_type(const char* name) const
{
  // 未完
  return nullptr;
}

// @brief このライブラリの持つセル数の取得
ymuint
CiLibrary::cell_num() const
{
  return mCellNum;
}

// @brief セルの取得
// @param[in] pos 位置番号( 0 <= pos < cell_num() )
const Cell*
CiLibrary::cell(ymuint pos) const
{
  ASSERT_COND( pos < cell_num() );
  return mCellArray[pos];
}

// @brief 名前からのセルの取得
const Cell*
CiLibrary::cell(const char* name) const
{
  return mCellHash.get(ShString(name));
}

// @brief 名前からのセルの取得
const Cell*
CiLibrary::cell(const string& name) const
{
  return mCellHash.get(ShString(name));
}

// @brief セルグループの個数を返す．
ymuint
CiLibrary::group_num() const
{
  return mGroupNum;
}

// @brief セルグループを返す．
// @param[in] id グループ番号　( 0 <= id < group_num() )
const CellGroup*
CiLibrary::group(ymuint id) const
{
  ASSERT_COND( id < group_num() );
  return &mGroupArray[id];
}

// @brief NPN同値クラスの個数を返す．
ymuint
CiLibrary::npn_class_num() const
{
  return mClassNum;
}

// @brief NPN同値クラスを返す．
// @param[in] id クラス番号 ( 0 <= id < npn_class_num() )
const CellClass*
CiLibrary::npn_class(ymuint id) const
{
  ASSERT_COND( id < npn_class_num() );
  return &mClassArray[id];
}

// @brief 定数0セルのグループを返す．
const CellGroup*
CiLibrary::const0_func() const
{
  // 決め打ち
  return mLogicGroup[0];
}

// @brief 定数1セルのグループを返す．
const CellGroup*
CiLibrary::const1_func() const
{
  // 決め打ち
  return mLogicGroup[1];
}

// @brief バッファセルのグループを返す．
const CellGroup*
CiLibrary::buf_func() const
{
  // 決め打ち
  return mLogicGroup[2];
}

// @brief インバータセルのグループを返す．
const CellGroup*
CiLibrary::inv_func() const
{
  // 決め打ち
  return mLogicGroup[3];
}

// @brief 単純な型のFFクラスを返す．
// @param[in] has_clear クリア端子を持つとき true にする．
// @param[in] has_preset プリセット端子を持つとき true にする．
// @note 該当するセルがないときでも空のセルクラスが返される．
const CellClass*
CiLibrary::simple_ff_class(bool has_clear,
			   bool has_preset) const
{
  ymuint pos = 0;
  if ( has_clear ) {
    pos += 1;
  }
  if ( has_preset ) {
    pos += 2;
  }
  return mFFClass[pos];
}

// @brief 単純な型のラッチクラスを返す．
// @param[in] has_clear クリア端子を持つとき true にする．
// @param[in] has_preset プリセット端子を持つとき true にする．
// @note 該当するセルがないときでも空のセルクラスが返される．
const CellClass*
CiLibrary::simple_latch_class(bool has_clear,
			      bool has_preset) const
{
  ymuint pos = 0;
  if ( has_clear ) {
    pos += 1;
  }
  if ( has_preset ) {
    pos += 2;
  }
  return mLatchClass[pos];
}

// @brief 総パタン数を返す．
ymuint
CiLibrary::pg_pat_num() const
{
  return mPatMgr.pat_num();
}

// @brief パタンを返す．
// @param[in] id パタン番号 ( 0 <= id < pg_pat_num() )
const CellPatGraph&
CiLibrary::pg_pat(ymuint id) const
{
  return mPatMgr.pat(id);
}

// @brief パタンの最大の入力数を得る．
ymuint
CiLibrary::pg_max_input() const
{
  return mPatMgr.max_input();
}

// @brief 総ノード数を返す．
ymuint
CiLibrary::pg_node_num() const
{
  return mPatMgr.node_num();
}

// @brief ノードの種類を返す．
// @param[in] id ノード番号 ( 0 <= id < pg_node_num() )
tCellPatType
CiLibrary::pg_node_type(ymuint id) const
{
  return mPatMgr.node_type(id);
}

// @brief ノードが入力ノードの時に入力番号を返す．
// @param[in] id ノード番号 ( 0 <= id < pg_node_num() )
// @note 入力ノードでない場合の返り値は不定
ymuint
CiLibrary::pg_input_id(ymuint id) const
{
  return mPatMgr.input_id(id);
}

// @brief 入力のノード番号を返す．
// @param[in] input_id 入力番号 ( 0 <= input_id < pg_input_num() )
// @return input_id の入力に対応するノードのノード番号
ymuint
CiLibrary::pg_input_node(ymuint input_id) const
{
  return mPatMgr.input_node(input_id);
}

// @brief 総枝数を返す．
ymuint
CiLibrary::pg_edge_num() const
{
  return mPatMgr.edge_num();
}

// @brief 枝のファンイン元のノード番号を返す．
// @param[in] id 枝番号 ( 0 <= id < pg_edge_num() )
ymuint
CiLibrary::pg_edge_from(ymuint id) const
{
  return mPatMgr.edge_from(id);
}

// @brief 枝のファンアウト先のノード番号を返す．
// @param[in] id 枝番号 ( 0 <= id < pg_edge_num() )
ymuint
CiLibrary::pg_edge_to(ymuint id) const
{
  return mPatMgr.edge_to(id);
}

// @brief 枝のファンアウト先の入力位置( 0 or 1 ) を返す．
// @param[in] id 枝番号 ( 0 <= id < pg_edge_num() )
ymuint
CiLibrary::pg_edge_pos(ymuint id) const
{
  return mPatMgr.edge_pos(id);
}

// @brief 枝の反転属性を返す．
// @param[in] id 枝番号 ( 0 <= id < pg_edge_num() )
bool
CiLibrary::pg_edge_inv(ymuint id) const
{
  return mPatMgr.edge_inv(id);
}

// @brief 名前を設定する．
// @param[in] name 名前
void
CiLibrary::set_name(const string& name)
{
  mName = name;
}

// @brief 'technology' を設定する．
void
CiLibrary::set_technology(tCellTechnology technology)
{
  mTechnology = technology;
}

// @brief 遅延モデルを設定する．
void
CiLibrary::set_delay_model(tCellDelayModel delay_model)
{
  mDelayModel = delay_model;
}

// @brief 'capacitive_load_unit' を設定する．
// @param[in] unit 単位
// @param[in] ustr 単位の後に表示する文字列
void
CiLibrary::set_capacitive_load_unit(double unit,
				    const string& ustr)
{
  mCapacitiveLoadUnit = unit;
  mCapacitiveLoadUnitStr = ustr;
}

// @brief 属性を設定する(浮動小数点型)
// @param[in] attr_name 属性名
// @param[in] value 値
void
CiLibrary::set_attr(const string& attr_name,
		    double value)
{
  if ( attr_name == "em_temp_degradation_factor" ) {
  }
  else if ( attr_name == "nom_process" ) {
  }
  else if ( attr_name == "nom_temperature" ) {
  }
  else if ( attr_name == "nom_voltage" ) {
  }
  else if ( attr_name == "default_cell_leakage_power" ) {
  }
  else if ( attr_name == "default_fall_delay_intercept" ) {
  }
  else if ( attr_name == "default_fall_pin_resistance" ) {
  }
  else if ( attr_name == "default_fanout_load" ) {
  }
  else if ( attr_name == "default_inout_pin_cap" ) {
  }
  else if ( attr_name == "default_inout_pin_fall_res" ) {
  }
  else if ( attr_name == "default_inout_pin_rise_res" ) {
  }
  else if ( attr_name == "default_inout_pin_cap" ) {
  }
  else if ( attr_name == "default_intrinsic_fall" ) {
  }
  else if ( attr_name == "default_intrinsic_rise" ) {
  }
  else if ( attr_name == "default_leakage_power_density" ) {
  }
  else if ( attr_name == "default_max_capacitance" ) {
  }
  else if ( attr_name == "default_max_fanout" ) {
  }
  else if ( attr_name == "default_max_transition" ) {
  }
  else if ( attr_name == "default_max_utilization" ) {
  }
  else if ( attr_name == "default_min_porosity" ) {
  }
  else if ( attr_name == "default_output_pin_cap" ) {
  }
  else if ( attr_name == "default_output_pin_fall_res" ) {
  }
  else if ( attr_name == "default_output_pin_rise_res" ) {
  }
  else if ( attr_name == "default_rise_delay_intercept" ) {
  }
  else if ( attr_name == "default_slope_fall" ) {
  }
  else if ( attr_name == "default_slope_rise" ) {
  }
  else if ( attr_name == "default_wire_load_area" ) {
  }
  else if ( attr_name == "default_wire_load_capacitance" ) {
  }
  else if ( attr_name == "default_wire_load_resistance" ) {
  }
}

// @brief 属性を設定する(文字列型)．
// @param[in] attr_name 属性名
// @param[in] value 値
void
CiLibrary::set_attr(const string& attr_name,
		    const string& value)
{
  if ( attr_name == "bus_naming_style" ) {
    mBusNamingStyle = value;
  }
  else if ( attr_name == "date" ) {
    mDate = value;
  }
  else if ( attr_name == "revision" ) {
    mRevision = value;
  }
  else if ( attr_name == "comment" ) {
    mComment = value;
  }
  else if ( attr_name == "time_unit" ) {
    mTimeUnit = value;
  }
  else if ( attr_name == "voltage_unit" ) {
    mVoltageUnit = value;
  }
  else if ( attr_name == "current_unit" ) {
    mCurrentUnit = value;
  }
  else if ( attr_name == "pulling_resistance_unit" ) {
    mPullingResistanceUnit = value;
  }
  else if ( attr_name == "leakage_power_unit" ) {
    mLeakagePowerUnit = value;
  }
}

// @brief 遅延テーブルのテンプレート数を設定する．
void
CiLibrary::set_lu_table_template_num(ymuint num)
{
  ASSERT_COND( mLutTemplateNum == 0 );
  void* p = mAlloc.get_memory(sizeof(CellLutTemplate*) * num);
  mLutTemplateNum = num;
  mLutTemplateArray = new (p) CiLutTemplate*[num];
}

// @brief 1次元の LUT のテンプレートを作る．
// @param[in] id ID 番号
// @param[in] name 名前
// @param[in] var_type1 変数型
// @param[in] index_list1 インデックス値のリスト
void
CiLibrary::new_lut_template1(ymuint id,
			     const string& name,
			     tCellVarType var_type1,
			     const vector<double>& index_list1)
{
  ASSERT_COND( id < lu_table_template_num() );
  void* p = mAlloc.get_memory(sizeof(CiLutTemplate1D));
  CiLutTemplate* tmpl = new (p) CiLutTemplate1D(ShString(name),
						var_type1, index_list1);
  mLutTemplateArray[id] = tmpl;
  mLutHash.add(tmpl);
}

// @brief 2次元の LUT のテンプレートを作る．
// @param[in] id ID 番号
// @param[in] name 名前
// @param[in] var_type1 変数型
// @param[in] index_list1 インデックス値のリスト
// @param[in] var_type2 変数型
// @param[in] index_list2 インデックス値のリスト
void
CiLibrary::new_lut_template2(ymuint id,
			     const string& name,
			     tCellVarType var_type1,
			     const vector<double>& index_list1,
			     tCellVarType var_type2,
			     const vector<double>& index_list2)
{
  ASSERT_COND( id < lu_table_template_num() );
  void* p = mAlloc.get_memory(sizeof(CiLutTemplate2D));
  CiLutTemplate* tmpl = new (p) CiLutTemplate2D(ShString(name),
						  var_type1, index_list1,
						  var_type2, index_list2);
  mLutTemplateArray[id] = tmpl;
  mLutHash.add(tmpl);
}

// @brief 3次元の LUT のテンプレートを作る．
// @param[in] id ID 番号
// @param[in] name 名前
// @param[in] var_type1 変数型
// @param[in] index_list1 インデックス値のリスト
// @param[in] var_type2 変数型
// @param[in] index_list2 インデックス値のリスト
// @param[in] var_type3 変数型
// @param[in] index_list3 インデックス値のリスト
void
CiLibrary::new_lut_template3(ymuint id,
			     const string& name,
			     tCellVarType var_type1,
			     const vector<double>& index_list1,
			     tCellVarType var_type2,
			     const vector<double>& index_list2,
			     tCellVarType var_type3,
			     const vector<double>& index_list3)
{
  ASSERT_COND( id < lu_table_template_num() );
  void* p = mAlloc.get_memory(sizeof(CiLutTemplate3D));
  CiLutTemplate* tmpl = new (p) CiLutTemplate3D(ShString(name),
						var_type1, index_list1,
						var_type2, index_list2,
						var_type3, index_list3);
  mLutTemplateArray[id] = tmpl;
  mLutHash.add(tmpl);
}

// @brief セル数を設定する．
// @param[in] num 設定する値
void
CiLibrary::set_cell_num(ymuint num)
{
  ASSERT_COND( mCellNum == 0 );
  mCellNum = num;
  void* p = mAlloc.get_memory(sizeof(CiCell*) * num);
  mCellArray = new (p) CiCell*[num];
}

// @brief セルを取り出す．
// @param[in] pos 位置番号 ( 0 <= pos < cell_num() )
Cell*
CiLibrary::cell(ymuint pos)
{
  ASSERT_COND( pos < mCellNum );
  return mCellArray[pos];
}

// @brief 論理セルを生成する．
// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
// @param[in] name 名前
// @param[in] area 面積
// @param[in] ni 入力ピン数
// @param[in] no 出力ピン数
// @param[in] nio 入出力ピン数
// @param[in] nb バス数
// @param[in] nc バンドル数
// @param[in] output_array 出力の情報の配列(*1)
// @param[in] logic_array 出力の論理式の配列
// @param[in] tristate_array トライステート条件の論理式の配列
// *1: - false 論理式なし
//     - true 論理式あり
void
CiLibrary::new_logic_cell(ymuint cell_id,
			  const string& name,
			  CellArea area,
			  ymuint ni,
			  ymuint no,
			  ymuint nio,
			  ymuint nb,
			  ymuint nc,
			  const vector<bool>& output_array,
			  const vector<Expr>& logic_array,
			  const vector<Expr>& tristate_array)
{
  void* p = mAlloc.get_memory(sizeof(CiLogicCell));
  CiCell* cell = new (p) CiLogicCell(this, cell_id, ShString(name), area,
				     ni, no, nio, nb, nc,
				     output_array,
				     logic_array,
				     tristate_array,
				     mAlloc);
  add_cell(cell_id, cell);
}

// @brief FFセルを生成する．
// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
// @param[in] name 名前
// @param[in] area 面積
// @param[in] ni 入力ピン数
// @param[in] no 出力ピン数
// @param[in] nio 入出力ピン数
// @param[in] nb バス数
// @param[in] nc バンドル数
// @param[in] output_array 出力の情報の配列(*1)
// @param[in] logic_array 出力の論理式の配列
// @param[in] tristate_array トライステート条件の論理式の配列
// @param[in] next_state "next_state" 関数の式
// @param[in] clocked_on "clocked_on" 関数の式
// @param[in] clocked_on_also "clocked_on_also" 関数の式
// @param[in] clear "clear" 関数の式
// @param[in] preset "preset" 関数の式
// *1: - false 論理式なし
//     - true 論理式あり
void
CiLibrary::new_ff_cell(ymuint cell_id,
		       const string& name,
		       CellArea area,
		       ymuint ni,
		       ymuint no,
		       ymuint nio,
		       ymuint nb,
		       ymuint nc,
		       const vector<bool>& output_array,
		       const vector<Expr>& logic_array,
		       const vector<Expr>& tristate_array,
		       const Expr& next_state,
		       const Expr& clocked_on,
		       const Expr& clocked_on_also,
		       const Expr& clear,
		       const Expr& preset,
		       ymuint clear_preset_var1,
		       ymuint clear_preset_var2)
{
  bool has_clear = !clear.is_zero();
  bool has_preset = !preset.is_zero();

  ShString shname(name);

  CiCell* cell = nullptr;
  if ( has_clear ) {
    if ( has_preset ) {
      void* p = mAlloc.get_memory(sizeof(CiFFSRCell));
      cell = new (p) CiFFSRCell(this, cell_id, shname, area,
				ni, no, nio, nb, nc,
				output_array,
				logic_array,
				tristate_array,
				next_state,
				clocked_on,
				clocked_on_also,
				clear,
				preset,
				clear_preset_var1,
				clear_preset_var2,
				mAlloc);
    }
    else {
      void* p = mAlloc.get_memory(sizeof(CiFFRCell));
      cell = new (p) CiFFRCell(this, cell_id, shname, area,
			       ni, no, nio, nb, nc,
			       output_array,
			       logic_array,
			       tristate_array,
			       next_state,
			       clocked_on,
			       clocked_on_also,
			       clear,
			       mAlloc);
    }
  }
  else {
    if ( has_preset ) {
      void* p = mAlloc.get_memory(sizeof(CiFFSCell));
      cell = new (p) CiFFSCell(this, cell_id, shname, area,
			       ni, no, nio, nb, nc,
			       output_array,
			       logic_array,
			       tristate_array,
			       next_state,
			       clocked_on,
			       clocked_on_also,
			       preset,
			       mAlloc);
    }
    else {
      void* p = mAlloc.get_memory(sizeof(CiFFCell));
      cell = new (p) CiFFCell(this, cell_id, shname, area,
			      ni, no, nio, nb, nc,
			      output_array,
			      logic_array,
			      tristate_array,
			      next_state,
			      clocked_on,
			      clocked_on_also,
			      mAlloc);
    }
  }
  add_cell(cell_id, cell);
}

// @brief ラッチセルを生成する．
// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
// @param[in] name 名前
// @param[in] area 面積
// @param[in] ni 入力ピン数
// @param[in] no 出力ピン数
// @param[in] nio 入出力ピン数
// @param[in] nb バス数
// @param[in] nc バンドル数
// @param[in] output_array 出力の情報の配列(*1)
// @param[in] logic_array 出力の論理式の配列
// @param[in] tristate_array トライステート条件の論理式の配列
// @param[in] data_in "data_in" 関数の式
// @param[in] enable "enable" 関数の式
// @param[in] enable_also "enable_also" 関数の式
// @param[in] clear "clear" 関数の式
// @param[in] preset "preset" 関数の式
// @param[in] clear_preset_var1 clear と preset が同時にオンになったときの値1
// @param[in] clear_preset_var2 clear と preset が同時にオンになったときの値2
// *1: - false 論理式なし
//     - true 論理式あり
void
CiLibrary::new_latch_cell(ymuint cell_id,
			  const string& name,
			  CellArea area,
			  ymuint ni,
			  ymuint no,
			  ymuint nio,
			  ymuint nb,
			  ymuint nc,
			  const vector<bool>& output_array,
			  const vector<Expr>& logic_array,
			  const vector<Expr>& tristate_array,
			  const Expr& data_in,
			  const Expr& enable,
			  const Expr& enable_also,
			  const Expr& clear,
			  const Expr& preset,
			  ymuint clear_preset_var1,
			  ymuint clear_preset_var2)
{
  bool has_clear = !clear.is_zero();
  bool has_preset = !preset.is_zero();

  ShString shname(name);

  CiCell* cell = nullptr;
  if ( has_clear ) {
    if ( has_preset ) {
      void* p = mAlloc.get_memory(sizeof(CiLatchSRCell));
      cell = new (p) CiLatchSRCell(this, cell_id, shname, area,
				   ni, no, nio, nb, nc,
				   output_array,
				   logic_array,
				   tristate_array,
				   data_in,
				   enable,
				   enable_also,
				   clear,
				   preset,
				   clear_preset_var1,
				   clear_preset_var2,
				   mAlloc);
    }
    else {
      void* p = mAlloc.get_memory(sizeof(CiLatchRCell));
      cell = new (p) CiLatchRCell(this, cell_id, shname, area,
				  ni, no, nio, nb, nc,
				  output_array,
				  logic_array,
				  tristate_array,
				  data_in,
				  enable,
				  enable_also,
				  clear,
				  mAlloc);
    }
  }
  else {
    if ( has_preset ) {
      void* p = mAlloc.get_memory(sizeof(CiLatchSCell));
      cell = new (p) CiLatchSCell(this, cell_id, shname, area,
				  ni, no, nio, nb, nc,
				  output_array,
				  logic_array,
				  tristate_array,
				  data_in,
				  enable,
				  enable_also,
				  preset,
				  mAlloc);
    }
    else {
      void* p = mAlloc.get_memory(sizeof(CiLatchCell));
      cell = new (p) CiLatchCell(this, cell_id, shname, area,
				 ni, no, nio, nb, nc,
				 output_array,
				 logic_array,
				 tristate_array,
				 data_in,
				 enable,
				 enable_also,
				 mAlloc);
    }
  }
  add_cell(cell_id, cell);
}

// @brief FSMセルを生成する．
// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
// @param[in] name 名前
// @param[in] area 面積
// @param[in] ni 入力ピン数
// @param[in] no 出力ピン数
// @param[in] nio 入出力ピン数
// @param[in] nit 内部ピン数
// @param[in] nb バス数
// @param[in] nc バンドル数
// @param[in] output_array 出力の情報の配列(*1)
// @param[in] logic_array 出力の論理式の配列
// @param[in] tristate_array トライステート条件の論理式の配列
// *1: - false 論理式なし
//     - true 論理式あり
void
CiLibrary::new_fsm_cell(ymuint cell_id,
			const string& name,
			CellArea area,
			ymuint ni,
			ymuint no,
			ymuint nio,
			ymuint nit,
			ymuint nb,
			ymuint nc,
			const vector<bool>& output_array,
			const vector<Expr>& logic_array,
			const vector<Expr>& tristate_array)
{
  void* p = mAlloc.get_memory(sizeof(CiFsmCell));
  CiCell* cell = new (p) CiFsmCell(this, cell_id, ShString(name), area,
				   ni, no, nio, nit, nb, nc,
				   output_array,
				   logic_array,
				   tristate_array,
				   mAlloc);
  add_cell(cell_id, cell);
}

// @brief セルの入力ピンの内容を設定する．
// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
// @param[in] pin_id ピン番号 ( 0 <= pin_id < cell->pin_num() )
// @param[in] input_id 入力ピン番号 ( 0 <= input_id < cell->input_num2() )
// @param[in] name 入力ピン名
// @param[in] capacitance 入力ピンの負荷容量
// @param[in] rise_capacitance 入力ピンの立ち上がり負荷容量
// @param[in] fall_capacitance 入力ピンの立ち下がり負荷容量
void
CiLibrary::new_cell_input(ymuint cell_id,
			  ymuint pin_id,
			  ymuint input_id,
			  const string& name,
			  CellCapacitance capacitance,
			  CellCapacitance rise_capacitance,
			  CellCapacitance fall_capacitance)
{
  CiCell* cell = mCellArray[cell_id];
  void* p = mAlloc.get_memory(sizeof(CiInputPin));
  CiInputPin* pin = new (p) CiInputPin(cell, ShString(name),
				       capacitance,
				       rise_capacitance, fall_capacitance);
  cell->mPinArray[pin_id] = pin;
  pin->mId = pin_id;
  cell->mInputArray[input_id] = pin;
  pin->mInputId = input_id;
  mPinHash.add(pin);
}

// @brief セルの出力ピンの内容を設定する．
// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
// @param[in] pin_id ピン番号 ( 0 <= pin_id < cell->pin_num() )
// @param[in] output_id 出力ピン番号 ( 0 <= output_id < cell->output_num2() )
// @param[in] name 出力ピン名
// @param[in] has_logic 論理式を持つとき true にするフラグ
// @param[in] logic_expr 論理式
// @param[in] tristate_expr tristate 条件式
// @param[in] max_fanout 最大ファンアウト容量
// @param[in] min_fanout 最小ファンアウト容量
// @param[in] max_capacitance 最大負荷容量
// @param[in] min_capacitance 最小負荷容量
// @param[in] max_transition 最大遷移時間
// @param[in] min_transition 最小遷移時間
void
CiLibrary::new_cell_output(ymuint cell_id,
			   ymuint pin_id,
			   ymuint output_id,
			   const string& name,
			   bool has_logic,
			   const Expr& logic_expr,
			   const Expr& tristate_expr,
			   CellCapacitance max_fanout,
			   CellCapacitance min_fanout,
			   CellCapacitance max_capacitance,
			   CellCapacitance min_capacitance,
			   CellTime max_transition,
			   CellTime min_transition)
{
  CiCell* cell = mCellArray[cell_id];
  void* p = mAlloc.get_memory(sizeof(CiOutputPin));
  CiOutputPin* pin = new (p) CiOutputPin(cell, ShString(name),
					 has_logic, logic_expr,
					 tristate_expr,
					 max_fanout, min_fanout,
					 max_capacitance, min_capacitance,
					 max_transition, min_transition);
  cell->mPinArray[pin_id] = pin;
  pin->mId = pin_id;
  cell->mOutputArray[output_id] = pin;
  pin->mOutputId = output_id;
  mPinHash.add(pin);
}

// @brief セルの入出力ピンの内容を設定する．
// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
// @param[in] pin_id 入出力ピン番号 ( 0 <= pin_id < cell->pin_num() )
// @param[in] input_id 入力ピン番号 ( 0 <= input_id < cell->input_num2() )
// @param[in] output_id 出力ピン番号 ( 0 <= output_id < cell->output_num2() )
// @param[in] name 入出力ピン名
// @param[in] has_logic 論理式を持つとき true にするフラグ
// @param[in] logic_expr 論理式
// @param[in] tristate_expr tristate 条件式
// @param[in] capacitance 入力ピンの負荷容量
// @param[in] rise_capacitance 入力ピンの立ち上がり負荷容量
// @param[in] fall_capacitance 入力ピンの立ち下がり負荷容量
// @param[in] max_fanout 最大ファンアウト容量
// @param[in] min_fanout 最小ファンアウト容量
// @param[in] max_capacitance 最大負荷容量
// @param[in] min_capacitance 最小負荷容量
// @param[in] max_transition 最大遷移時間
// @param[in] min_transition 最小遷移時間
void
CiLibrary::new_cell_inout(ymuint cell_id,
			  ymuint pin_id,
			  ymuint input_id,
			  ymuint output_id,
			  const string& name,
			  bool has_logic,
			  const Expr& logic_expr,
			  const Expr& tristate_expr,
			  CellCapacitance capacitance,
			  CellCapacitance rise_capacitance,
			  CellCapacitance fall_capacitance,
			  CellCapacitance max_fanout,
			  CellCapacitance min_fanout,
			  CellCapacitance max_capacitance,
			  CellCapacitance min_capacitance,
			  CellTime max_transition,
			  CellTime min_transition)
{
  CiCell* cell = mCellArray[cell_id];
  void* p = mAlloc.get_memory(sizeof(CiInoutPin));
  CiInoutPin* pin =  new (p) CiInoutPin(cell, ShString(name),
					has_logic, logic_expr,
					tristate_expr,
					capacitance,
					rise_capacitance, fall_capacitance,
					max_fanout, min_fanout,
					max_capacitance, min_capacitance,
					max_transition, min_transition);
  cell->mPinArray[pin_id] = pin;
  pin->mId = pin_id;
  cell->mInputArray[input_id] = pin;
  pin->mInputId = input_id;
  cell->mOutputArray[output_id] = pin;
  pin->mOutputId = output_id;
  mPinHash.add(pin);
}

// @brief セルの内部ピンを生成する．
// @param[in] cell_id セル番号
// @param[in] pin_id ピン番号 ( 0 <= pin_id < cell->pin_num() )
// @param[in] internal_id 入力ピン番号 ( 0 <= internal_id < cell->internal_num() )
// @param[in] name 内部ピン名
void
CiLibrary::new_cell_internal(ymuint cell_id,
			     ymuint pin_id,
			     ymuint internal_id,
			     const string& name)
{
  CiCell* cell = mCellArray[cell_id];
  void* p = mAlloc.get_memory(sizeof(CiInternalPin));
  CiInternalPin* pin = new (p) CiInternalPin(cell, ShString(name));
  cell->mPinArray[pin_id] = pin;
  pin->mId = pin_id;
  cell->mInternalArray[internal_id] = pin;
  pin->mInternalId = internal_id;
}

// @brief タイミング情報の数を設定する．
// @param[in] cell_id セル番号
// @param[in] timing_num タイミング情報の数．
void
CiLibrary::set_timing_num(ymuint cell_id,
			  ymuint timing_num)
{
  CiCell* cell = mCellArray[cell_id];
  cell->mTimingNum = timing_num;
  void* p = mAlloc.get_memory(sizeof(CiTiming*) * timing_num);
  cell->mTimingArray = new (p) CiTiming*[timing_num];
}

// @brief タイミング情報を作る(ジェネリック遅延モデル)．
// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
// @param[in] tid タイミングID ( 0 <= tid < cell->timing_num() )
// @param[in] type タイミングの型
// @param[in] cond タイミング条件を表す式
// @param[in] intrinsic_rise 立ち上がり固有遅延
// @param[in] intrinsic_fall 立ち下がり固有遅延
// @param[in] slope_rise 立ち上がりスロープ遅延
// @param[in] slope_fall 立ち下がりスロープ遅延
// @param[in] rise_resistance 立ち上がり負荷依存係数
// @param[in] fall_resistance 立ち下がり負荷依存係数
void
CiLibrary::new_timing_generic(ymuint cell_id,
			      ymuint tid,
			      tCellTimingType type,
			      const Expr& cond,
			      CellTime intrinsic_rise,
			      CellTime intrinsic_fall,
			      CellTime slope_rise,
			      CellTime slope_fall,
			      CellResistance rise_resistance,
			      CellResistance fall_resistance)
{
  void* p = mAlloc.get_memory(sizeof(CiTimingGeneric));
  CiTiming* timing = new (p) CiTimingGeneric(tid, type, cond,
					     intrinsic_rise,
					     intrinsic_fall,
					     slope_rise,
					     slope_fall,
					     rise_resistance,
					     fall_resistance);
  CiCell* cell = mCellArray[cell_id];
  cell->mTimingArray[tid] = timing;
}

// @brief タイミング情報を作る(折れ線近似)．
// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
// @param[in] tid タイミングID ( 0 <= tid < cell->timing_num() )
// @param[in] timing_type タイミングの型
// @param[in] cond タイミング条件を表す式
// @param[in] intrinsic_rise 立ち上がり固有遅延
// @param[in] intrinsic_fall 立ち下がり固有遅延
// @param[in] slope_rise 立ち上がりスロープ遅延
// @param[in] slope_fall 立ち下がりスロープ遅延
void
CiLibrary::new_timing_piecewise(ymuint cell_id,
				ymuint tid,
				tCellTimingType timing_type,
				const Expr& cond,
				CellTime intrinsic_rise,
				CellTime intrinsic_fall,
				CellTime slope_rise,
				CellTime slope_fall,
				CellResistance rise_pin_resistance,
				CellResistance fall_pin_resistance)
{
  void* p = mAlloc.get_memory(sizeof(CiTimingPiecewise));
  CiTiming* timing = new (p) CiTimingPiecewise(tid, timing_type, cond,
					       intrinsic_rise,
					       intrinsic_fall,
					       slope_rise,
					       slope_fall,
					       rise_pin_resistance,
					       fall_pin_resistance);
  CiCell* cell = mCellArray[cell_id];
  cell->mTimingArray[tid] = timing;
}

// @brief タイミング情報を作る(非線形タイプ1)．
// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
// @param[in] tid タイミングID ( 0 <= tid < cell->timing_num() )
// @param[in] timing_type タイミングの型
// @param[in] cond タイミング条件を表す式
// @param[in] cell_rise 立ち上がりセル遅延テーブル
// @param[in] cell_fall 立ち下がりセル遅延テーブル
void
CiLibrary::new_timing_lut1(ymuint cell_id,
			   ymuint tid,
			   tCellTimingType timing_type,
			   const Expr& cond,
			   CellLut* cell_rise,
			   CellLut* cell_fall,
			   CellLut* rise_transition,
			   CellLut* fall_transition)
{
  void* p = mAlloc.get_memory(sizeof(CiTimingLut1));
  CiTiming* timing = new (p) CiTimingLut1(tid, timing_type, cond,
					  cell_rise,
					  cell_fall,
					  rise_transition,
					  fall_transition);
  CiCell* cell = mCellArray[cell_id];
  cell->mTimingArray[tid] = timing;
}

// @brief タイミング情報を作る(非線形タイプ2)．
// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
// @param[in] tid タイミングID ( 0 <= tid < cell->timing_num() )
// @param[in] timing_type タイミングの型
// @param[in] cond タイミング条件を表す式
// @param[in] rise_transition 立ち上がり遷移遅延テーブル
// @param[in] fall_transition 立ち下がり遷移遅延テーブル
// @param[in] rise_propagation 立ち上がり伝搬遅延テーブル
// @param[in] fall_propagation 立ち下がり伝搬遅延テーブル
void
CiLibrary::new_timing_lut2(ymuint cell_id,
			   ymuint tid,
			   tCellTimingType timing_type,
			   const Expr& cond,
			   CellLut* rise_transition,
			   CellLut* fall_transition,
			   CellLut* rise_propagation,
			   CellLut* fall_propagation)
{
  void* p = mAlloc.get_memory(sizeof(CiTimingLut2));
  CiTiming* timing = new (p) CiTimingLut2(tid, timing_type, cond,
					  rise_transition,
					  fall_transition,
					  rise_propagation,
					  fall_propagation);
  CiCell* cell = mCellArray[cell_id];
  cell->mTimingArray[tid] = timing;
}

// @brief タイミング情報をセットする．
// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
// @param[in] opin_id 出力(入出力)ピン番号 ( *1 )
// @param[in] ipin_id 関連する入力(入出力)ピン番号 ( *2 )
// @param[in] timing_sense タイミングセンス
// @param[in] tid_list タイミングIDのリスト
// @note ( *1 ) opin_id で入出力ピンを表す時には入出力ピン番号
//  + cell->output_num() を使う．
// @note ( *2 ) ipin_id で入出力ピンを表す時には入出力ピン番号
//  + cell->input_num() を使う．
void
CiLibrary::set_timing(ymuint cell_id,
		      ymuint ipin_id,
		      ymuint opin_id,
		      tCellTimingSense timing_sense,
		      const vector<ymuint>& tid_list)
{
  CiCell* cell = mCellArray[cell_id];
  ymuint base = (opin_id * cell->input_num2() + ipin_id) * 2;
  switch ( timing_sense ) {
  case kCellPosiUnate: base += 0; break;
  case kCellNegaUnate: base += 1; break;
  default: ASSERT_NOT_REACHED;
  }

  ymuint n = tid_list.size();
  if ( n == 0 ) {
    cell->mTimingMap[base] = nullptr;
  }
  else {
    void* p = mAlloc.get_memory(sizeof(CiTimingArray) + sizeof(CiTiming*) * (n - 1));
    CiTimingArray* tarray = new (p) CiTimingArray;
    tarray->mNum = n;
    for (ymuint i = 0; i < n; ++ i) {
      ymuint tid = tid_list[i];
      tarray->mArray[i] = cell->mTimingArray[tid];
    }
    cell->mTimingMap[base] = tarray;
  }
}

// @brief 1次元の LUT を作る．
// @param[in] lut_template テンプレート
// @param[in] value_array 値の配列
// @param[in] index_array インデックス値のリスト
CellLut*
CiLibrary::new_lut1(const CellLutTemplate* lut_template,
		    const vector<double>& value_array,
		    const vector<double>& index_array)
{
  void* p = mAlloc.get_memory(sizeof(CiLut1D));
  CellLut* lut = new (p) CiLut1D(lut_template, value_array, index_array);
  return lut;
}

// @brief 2次元の LUT を作る．
// @param[in] lut_template テンプレート
// @param[in] value_array 値の配列
// @param[in] index_array1 インデックス値のリスト
// @param[in] index_array2 インデックス値のリスト
CellLut*
CiLibrary::new_lut2(const CellLutTemplate* lut_template,
		    const vector<double>& value_array,
		    const vector<double>& index_array1,
		    const vector<double>& index_array2)
{
  void* p = mAlloc.get_memory(sizeof(CiLut2D));
  CellLut* lut = new (p) CiLut2D(lut_template, value_array,
				 index_array1, index_array2);
  return lut;
}

// @brief 3次元の LUT を作る．
// @param[in] lut_template テンプレート
// @param[in] value_array 値の配列
// @param[in] index_array1 インデックス値のリスト
// @param[in] index_array2 インデックス値のリスト
// @param[in] index_array3 インデックス値のリスト
CellLut*
CiLibrary::new_lut3(const CellLutTemplate* lut_template,
		    const vector<double>& value_array,
		    const vector<double>& index_array1,
		    const vector<double>& index_array2,
		    const vector<double>& index_array3)
{
  void* p = mAlloc.get_memory(sizeof(CiLut3D));
  CellLut* lut = new (p) CiLut3D(lut_template, value_array,
				 index_array1, index_array2, index_array3);
  return lut;
}

// @brief セルのグループ分けを行う．
// @note 論理セルのパタングラフも作成する．
void
CiLibrary::compile()
{
  LibComp libcomp;

  libcomp.compile(*this);

  ymuint nc = libcomp.npn_class_num();
  set_class_num(nc);

  ymuint ng = libcomp.group_num();
  set_group_num(ng);
  for (ymuint g = 0; g < ng; ++ g) {
    const LcGroup* src_group = libcomp.group(g);
    CiGroup& dst_group = mGroupArray[g];
    const CellClass* parent = npn_class(src_group->parent()->id());
    const vector<Cell*>& cell_list = src_group->cell_list();
    dst_group.init(parent, src_group->map(), cell_list, mAlloc);
  }

  for (ymuint c = 0; c < nc; ++ c) {
    const LcClass* src_class = libcomp.npn_class(c);
    const vector<LcGroup*>& src_group_list = src_class->group_list();
    ymuint n = src_group_list.size();
    vector<const CellGroup*> dst_group_list(n);
    for (ymuint i = 0; i < n; ++ i) {
      const CellGroup* dst_group = group(src_group_list[i]->id());
      dst_group_list[i] = dst_group;
    }
    CiClass& dst_class = mClassArray[c];
    dst_class.init(src_class->idmap_list(), dst_group_list, mAlloc);
  }

  for (ymuint i = 0; i < 4; ++ i) {
    mLogicGroup[i] = &mGroupArray[libcomp.logic_group(i)];
  }

  for (ymuint i = 0; i < 4; ++ i) {
    CiClass* cclass = &mClassArray[libcomp.ff_class(i)];
    mFFClass[i] = cclass;
    ymuint n = cclass->group_num();
    for (ymuint g = 0; g < n; ++ g) {
      // ちょっと面倒な手順を踏む．
      ymuint gid = cclass->cell_group(g)->id();
      CiGroup* cgroup = &mGroupArray[gid];
      NpnMapM map = cgroup->map();
      ymuint pos_array[6] = { 0, 0, 0, 0, 0, 0 };
      ymuint ni = map.input_num() - 2;
      ASSERT_COND( ni <= 4 );
      for (ymuint i = 0; i < ni; ++ i) {
	NpnVmap imap = map.imap(VarId(i));
	ymuint pos = imap.var().val();
	ymuint pol = imap.inv() ? 16U : 8U;
	pos_array[pos] = i | pol;
      }
      pos_array[4] = 0;
      pos_array[5] = 1;
      cgroup->set_ff_info(pos_array);
    }
  }
  for (ymuint i = 0; i < 4; ++ i) {
    CiClass* cclass = &mClassArray[libcomp.latch_class(i)];
    mLatchClass[i] = cclass;
    ymuint n = cclass->group_num();
    for (ymuint g = 0; g < n; ++ g) {
      // ちょっと面倒な手順を踏む．
      ymuint gid = cclass->cell_group(g)->id();
      CiGroup* cgroup = &mGroupArray[gid];
      NpnMapM map = cgroup->map();
      ymuint pos_array[5] = { 0, 0, 0, 0, 0 };
      ymuint ni = map.input_num() - 2;
      ASSERT_COND( ni <= 4 );
      for (ymuint i = 0; i < ni; ++ i) {
	NpnVmap imap = map.imap(VarId(i));
	ymuint pos = imap.var().val();
	ymuint pol = imap.inv() ? 16U : 8U;
	pos_array[pos] = i | pol;
      }
      pos_array[4] = 0;
      cgroup->set_latch_info(pos_array);
    }
  }

  mPatMgr.copy(libcomp.pat_mgr(), mAlloc);
}

// @brief クラス数を設定する．
// @param[in] nc クラス数
// @note 同時にクラスの配列の確保を行う．
void
CiLibrary::set_class_num(ymuint nc)
{
  mClassNum = nc;
  void* p = mAlloc.get_memory(sizeof(CiClass) * mClassNum);
  mClassArray = new (p) CiClass[mClassNum];
  for (ymuint i = 0; i < mClassNum; ++ i) {
    mClassArray[i].mId = i;
  }
}

// @brief グループ数を設定する．
// @param[in] ng グループ数
// @note 同時にグループの配列の確保を行う．
void
CiLibrary::set_group_num(ymuint ng)
{
  mGroupNum = ng;
  void* p = mAlloc.get_memory(sizeof(CiGroup) * mGroupNum);
  mGroupArray = new (p) CiGroup[mGroupNum];
  for (ymuint i = 0; i < mGroupNum; ++ i) {
    mGroupArray[i].mId = i;
  }
}

// @brief セルを追加する．
// @param[in] id セル番号
// @param[in] cell セル
void
CiLibrary::add_cell(ymuint id,
		    CiCell* cell)
{
  mCellArray[id] = cell;
  mCellHash.add(cell);
}

/// @brief 内容をバイナリダンプする．
/// @param[in] s 出力先のストリーム
void
CiLibrary::dump(ODO& s) const
{
  // 名前
  s << name();

  // テクノロジ
  s << static_cast<ymuint8>(technology());

  // 遅延モデル
  s << static_cast<ymuint8>(delay_model());

  // バス命名規則
  s << bus_naming_style();

  // 日付情報
  s << date();

  // リビジョン情報
  s << revision();

  // コメント
  s << comment();

  // 時間単位
  s << time_unit();

  // 電圧単位
  s << voltage_unit();

  // 電流単位
  s << current_unit();

  // 抵抗単位
  s << pulling_resistance_unit();

  // 容量単位
  s << capacitive_load_unit();

  // 容量単位の文字列
  s << capacitive_load_unit_str();

  // 電力単位
  s << leakage_power_unit();

  // 遅延テーブルのテンプレート
  ymuint32 ntempl = lu_table_template_num();
  s << ntempl;
  for (ymuint i = 0; i < ntempl; ++ i) {
    lu_table_template(i)->dump(s);
  }

  // セル数
  ymuint32 nc = cell_num();
  s << nc;
  for (ymuint i = 0; i < nc; ++ i) {
    // セルの内容をダンプ
    cell(i)->dump(s);
  }

  // セルクラスの個数だけダンプする．
  s << mClassNum
    << mGroupNum;

  // セルグループ情報のダンプ
  for (ymuint g = 0; g < mGroupNum; ++ g) {
    mGroupArray[g].dump(s);
  }

  // セルクラス情報のダンプ
  for (ymuint i = 0; i < mClassNum; ++ i) {
    mClassArray[i].dump(s);
  }

  // 組み込み型の情報のダンプ
  for (ymuint i = 0; i < 4; ++ i) {
    ymuint32 group_id = mLogicGroup[i]->id();
    s << group_id;
  }
  for (ymuint i = 0; i < 4; ++ i) {
    ymuint32 class_id = mFFClass[i]->id();
    s << class_id;
  }
  for (ymuint i = 0; i < 4; ++ i) {
    ymuint32 class_id = mLatchClass[i]->id();
    s << class_id;
  }

  // パタングラフの情報のダンプ
  mPatMgr.dump(s);
}

void
CiLibrary::restore(IDO& s)
{
  string name;
  s >> name;

  set_name(name);

  ymuint8 tmp1;
  ymuint8 tmp2;
  s >> tmp1
    >> tmp2;
  tCellTechnology technology = static_cast<tCellTechnology>(tmp1);
  tCellDelayModel delay_model = static_cast<tCellDelayModel>(tmp2);

  set_technology(technology);
  set_delay_model(delay_model);

  string bus_naming_style;
  s >> bus_naming_style;

  set_attr("bus_naming_style", bus_naming_style);

  string date;
  s >> date;

  set_attr("date", date);


  string revision;
  s >> revision;

  set_attr("revision", revision);

  string comment;
  s >> comment;

  set_attr("comment", comment);

  string time_unit;
  s >> time_unit;

  set_attr("time_unit", time_unit);

  string voltage_unit;
  s >> voltage_unit;

  set_attr("voltage_unit", voltage_unit);

  string current_unit;
  s >> current_unit;

  set_attr("current_unit", current_unit);

  string pulling_resistance_unit;
  s >> pulling_resistance_unit;

  set_attr("pulling_resistance_unit", pulling_resistance_unit);

  double capacitive_unit;
  string capacitive_unit_str;
  s >> capacitive_unit
    >> capacitive_unit_str;

  set_capacitive_load_unit(capacitive_unit, capacitive_unit_str);

  string leakage_power_unit;
  s >> leakage_power_unit;

  set_attr("leakage_power_unit", leakage_power_unit);

  ymuint32 lut_num;
  s >> lut_num;
  set_lu_table_template_num(lut_num);
  for (ymuint i = 0; i < lut_num; ++ i) {
    restore_lut_template(s, i);
  }

  ymuint32 nc;
  s >> nc;
  set_cell_num(nc);

  for (ymuint cell_id = 0; cell_id < nc; ++ cell_id) {
    ymuint8 type;
    string name;
    CellArea area;
    ymuint32 ni;
    ymuint32 no;
    ymuint32 nio;
    ymuint32 nit;
    ymuint32 nbus;
    ymuint32 nbundle;
    s >> type
      >> name
      >> area
      >> ni
      >> no
      >> nio
      >> nit
      >> nbus
      >> nbundle;

    ymuint no2 = no + nio;
    vector<bool> has_logic(no2);
    vector<Expr> logic_array(no2);
    vector<Expr> tristate_array(no2);
    for (ymuint opos = 0; opos < no2; ++ opos) {
      bool tmp;
      s >> tmp
	>> logic_array[opos]
	>> tristate_array[opos];
      has_logic[opos] = tmp;
    }

    switch ( type ) {
    case 0: // kLogic
      new_logic_cell(cell_id, name, area,
		     ni, no, nio, nbus, nbundle,
		     has_logic,
		     logic_array,
		     tristate_array);
      break;

    case 1: // kFF
      {
	Expr next_state;
	Expr clocked_on;
	Expr clocked_on_also;
	Expr clear;
	Expr preset;
	ymuint8 clear_preset_var1;
	ymuint8 clear_preset_var2;
	s >> next_state
	  >> clocked_on
	  >> clocked_on_also
	  >> clear
	  >> preset
	  >> clear_preset_var1
	  >> clear_preset_var2;
	new_ff_cell(cell_id, name, area,
		    ni, no, nio, nbus, nbundle,
		    has_logic,
		    logic_array,
		    tristate_array,
		    next_state,
		    clocked_on, clocked_on_also,
		    clear, preset,
		    clear_preset_var1,
		    clear_preset_var2);
      }
      break;

    case 2: // kLatch
      {
	Expr data_in;
	Expr enable;
	Expr enable_also;
	Expr clear;
	Expr preset;
	ymuint8 clear_preset_var1;
	ymuint8 clear_preset_var2;
	s >> data_in
	  >> enable
	  >> enable_also
	  >> clear
	  >> preset
	  >> clear_preset_var1
	  >> clear_preset_var2;
	new_latch_cell(cell_id, name, area,
		       ni, no, nio, nbus, nbundle,
		       has_logic,
		       logic_array,
		       tristate_array,
		       data_in,
		       enable, enable_also,
		       clear, preset,
		       clear_preset_var1,
		       clear_preset_var2);
      }
      break;

    case 3: // kFSM
      new_fsm_cell(cell_id, name, area,
		   ni, no, nio, nit, nbus, nbundle,
		   has_logic,
		   logic_array,
		   tristate_array);
      break;

    default:
      ASSERT_NOT_REACHED;
      break;
    }

    // 入力ピンの設定
    for (ymuint iid = 0; iid < ni; ++ iid) {
      string name;
      ymuint32 pin_id;
      CellCapacitance cap;
      CellCapacitance r_cap;
      CellCapacitance f_cap;
      s >> name
	>> pin_id
	>> cap
	>> r_cap
	>> f_cap;
      new_cell_input(cell_id, pin_id, iid, name, cap, r_cap, f_cap);
    }

    // 出力ピンの設定
    for (ymuint oid = 0; oid < no; ++ oid) {
      string name;
      ymuint32 pin_id;
      CellCapacitance max_f;
      CellCapacitance min_f;
      CellCapacitance max_c;
      CellCapacitance min_c;
      CellTime max_t;
      CellTime min_t;
      s >> name
	>> pin_id
	>> max_f
	>> min_f
	>> max_c
	>> min_c
	>> max_t
	>> min_t;
      new_cell_output(cell_id, pin_id, oid, name,
		      has_logic[oid], logic_array[oid],
		      tristate_array[oid],
		      max_f, min_f,
		      max_c, min_c,
		      max_t, min_t);
    }

    // 入出力ピンの設定
    for (ymuint ioid = 0; ioid < nio; ++ ioid) {
      string name;
      ymuint32 pin_id;
      CellCapacitance cap;
      CellCapacitance r_cap;
      CellCapacitance f_cap;
      CellCapacitance max_f;
      CellCapacitance min_f;
      CellCapacitance max_c;
      CellCapacitance min_c;
      CellTime max_t;
      CellTime min_t;
      s >> name
	>> pin_id
	>> cap
	>> r_cap
	>> f_cap
	>> max_f
	>> min_f
	>> max_c
	>> min_c
	>> max_t
	>> min_t;
      new_cell_inout(cell_id, pin_id, ioid + ni, ioid + no, name,
		     has_logic[ioid], logic_array[ioid],
		     tristate_array[ioid],
		     cap, r_cap, f_cap,
		     max_f, min_f,
		     max_c, min_c,
		     max_t, min_t);
    }

    // 内部ピンの設定
    for (ymuint itid = 0; itid < nit; ++ itid) {
      string name;
      ymuint32 pin_id;
      s >> name
	>> pin_id;
      new_cell_internal(cell_id, pin_id, itid, name);
    }

    // タイミング情報の生成
    ymuint32 nt;
    s >> nt;
    set_timing_num(cell_id, nt);
    for (ymuint tid = 0; tid < nt; ++ tid) {
      ymuint8 ttype;
      ymuint8 tmp;
      Expr cond;
      s >> ttype
	>> tmp
	>> cond;
      tCellTimingType timing_type = static_cast<tCellTimingType>(tmp);

      switch ( ttype ) {
      case 0:
	{
	  CellTime i_r;
	  CellTime i_f;
	  CellTime s_r;
	  CellTime s_f;
	  CellResistance r_r;
	  CellResistance f_r;
	  s >> i_r
	    >> i_f
	    >> s_r
	    >> s_f
	    >> r_r
	    >> f_r;
	  new_timing_generic(cell_id, tid,
			     timing_type,
			     cond,
			     i_r, i_f,
			     s_r, s_f,
			     r_r, f_r);
	}
	break;

      case 1:
	{
#if 0
	  CellTime i_r;
	  CellTime i_f;
	  CellTime s_r;
	  CellTime s_f;
	  s >> i_r
	    >> i_f
	    >> s_r
	    >> s_f;
	  new_timing_piecewise(cell_id, tid,
			       timing_type,
			       cond,
			       i_r, i_f,
			       s_r, s_f);
#endif
	}
	break;

      case 2:
	{
	  CellLut* cell_rise = restore_lut(s);
	  CellLut* cell_fall = restore_lut(s);
	  CellLut* rise_transition = restore_lut(s);
	  CellLut* fall_transition = restore_lut(s);
	  new_timing_lut1(cell_id, tid,
			  timing_type,
			  cond,
			  cell_rise,
			  cell_fall,
			  rise_transition,
			  fall_transition);
	}
	break;

      case 3:
	{
	  CellLut* rise_transition = restore_lut(s);
	  CellLut* fall_transition = restore_lut(s);
	  CellLut* rise_propagation = restore_lut(s);
	  CellLut* fall_propagation = restore_lut(s);
	  new_timing_lut1(cell_id, tid,
			  timing_type,
			  cond,
			  rise_transition,
			  fall_transition,
			  rise_propagation,
			  fall_propagation);
	}
	break;

      default:
	ASSERT_NOT_REACHED;
	break;
      }
    }

    // タイミング情報の設定
    for (ymuint ipos = 0; ipos < ni + nio; ++ ipos) {
      for (ymuint opos = 0; opos < no + nio; ++ opos) {
	ymuint32 np;
	s >> np;
	vector<ymuint> tid_list;
	tid_list.reserve(np);
	for (ymuint i = 0; i < np; ++ i) {
	  ymuint32 tid;
	  s >> tid;
	  tid_list.push_back(tid);
	}
	set_timing(cell_id, ipos, opos, kCellPosiUnate, tid_list);

	ymuint32 nn;
	s >> nn;
	tid_list.clear();
	tid_list.reserve(nn);
	for (ymuint i = 0; i < nn; ++ i) {
	  ymuint32 tid;
	  s >> tid;
	  tid_list.push_back(tid);
	}
	set_timing(cell_id, ipos, opos, kCellNegaUnate, tid_list);
      }
    }
  }

  // セルクラス数とグループ数の取得
  ymuint32 ncc;
  ymuint32 ng;
  s >> ncc
    >> ng;
  set_class_num(ncc);
  set_group_num(ng);

  // セルグループ情報の設定
  for (ymuint g = 0; g < ng; ++ g) {
    mGroupArray[g].restore(s, *this, mAlloc);
  }

  // セルクラス情報の設定
  for (ymuint c = 0; c < ncc; ++ c) {
    mClassArray[c].restore(s, *this, mAlloc);
  }

  // 組み込み型の設定
  for (ymuint i = 0; i < 4; ++ i) {
    ymuint32 group_id;
    s >> group_id;
    mLogicGroup[i] = &mGroupArray[group_id];
  }
  for (ymuint i = 0; i < 4; ++ i) {
    ymuint32 class_id;
    s >> class_id;
    mFFClass[i] = &mClassArray[class_id];
  }
  for (ymuint i = 0; i < 4; ++ i) {
    ymuint32 class_id;
    s >> class_id;
    mLatchClass[i] = &mClassArray[class_id];
  }

  // パタングラフの情報の設定
  mPatMgr.restore(s, mAlloc);
}

// @brief ピンの登録
// @param[in] pin 登録するピン
void
CiLibrary::add_pin(CiPin* pin)
{
  mPinHash.add(pin);
}

// @brief ピン名からピンを取り出す．
// @param[in] cell セル
// @param[in] name ピン名
CiPin*
CiLibrary::get_pin(const CiCell* cell,
		   ShString name)
{
  return mPinHash.get(cell, name);
}

// @brief LUT テンプレートを読み込む．
void
CiLibrary::restore_lut_template(IDO& s,
				ymuint id)
{
  string name;
  ymuint8 d;
  s >> name
    >> d;
  switch ( d ) {
  case 1:
    {
      ymuint8 tmp;
      s >> tmp;
      tCellVarType var_type = static_cast<tCellVarType>(tmp);
      ymuint8 n;
      s >> n;
      vector<double> index_array(n);
      for (ymuint i = 0; i < n; ++ i) {
	double val;
	s >> val;
	index_array[i] = val;
      }

      new_lut_template1(id, name,
			var_type, index_array);
    }
    break;

  case 2:
    {
      ymuint8 tmp1;
      s >> tmp1;
      tCellVarType var_type1 = static_cast<tCellVarType>(tmp1);
      ymuint8 n1;
      s >> n1;
      vector<double> index_array1(n1);
      for (ymuint i = 0; i < n1; ++ i) {
	double val;
	s >> val;
	index_array1[i] = val;
      }

      ymuint8 tmp2;
      s >> tmp2;
      tCellVarType var_type2 = static_cast<tCellVarType>(tmp2);
      ymuint8 n2;
      s >> n2;
      vector<double> index_array2(n2);
      for (ymuint i = 0; i < n2; ++ i) {
	double val;
	s >> val;
	index_array2[i] = val;
      }

      new_lut_template2(id, name,
			var_type1, index_array1,
			var_type2, index_array2);
    }
    break;

  case 3:
    {
      ymuint8 tmp1;
      s >> tmp1;
      tCellVarType var_type1 = static_cast<tCellVarType>(tmp1);
      ymuint8 n1;
      s >> n1;
      vector<double> index_array1(n1);
      for (ymuint i = 0; i < n1; ++ i) {
	double val;
	s >> val;
	index_array1[i] = val;
      }

      ymuint8 tmp2;
      s >> tmp2;
      tCellVarType var_type2 = static_cast<tCellVarType>(tmp2);
      ymuint8 n2;
      s >> n2;
      vector<double> index_array2(n2);
      for (ymuint i = 0; i < n2; ++ i) {
	double val;
	s >> val;
	index_array2[i] = val;
      }

      ymuint8 tmp3;
      s >> tmp3;
      tCellVarType var_type3 = static_cast<tCellVarType>(tmp3);
      ymuint8 n3;
      s >> n3;
      vector<double> index_array3(n3);
      for (ymuint i = 0; i < n3; ++ i) {
	double val;
	s >> val;
	index_array3[i] = val;
      }

      new_lut_template3(id, name,
			var_type1, index_array1,
			var_type2, index_array2,
			var_type3, index_array3);
    }
    break;
  }
}

// @brief LUT を読み込む．
CellLut*
CiLibrary::restore_lut(IDO& s)
{
  string template_name;
  s >> template_name;
  if ( template_name == string() ) {
    return nullptr;
  }

  const CellLutTemplate* templ = lu_table_template(template_name.c_str());
  ASSERT_COND( templ != nullptr );

  ymuint d = templ->dimension();
  switch ( d ) {
  case 1:
    {
      ymuint8 n;
      s >> n;
      vector<double> index_array(n);
      for (ymuint i = 0; i < n; ++ i) {
	double val;
	s >> val;
	index_array[i] = val;
      }

      vector<double> value_array(n);
      for (ymuint i = 0; i < n; ++ i) {
	double val;
	s >> val;
	value_array[i] = val;
      }
      return new_lut1(templ,
		      value_array,
		      index_array);
    }

  case 2:
    {
      ymuint8 n1;
      s >> n1;
      vector<double> index_array1(n1);
      for (ymuint i = 0; i < n1; ++ i) {
	double val;
	s >> val;
	index_array1[i] = val;
      }

      ymuint8 n2;
      s >> n2;
      vector<double> index_array2(n2);
      for (ymuint i = 0; i < n2; ++ i) {
	double val;
	s >> val;
	index_array2[i] = val;
      }

      ymuint n = n1 * n2;
      vector<double> value_array(n);
      for (ymuint i = 0; i < n; ++ i) {
	double val;
	s >> val;
	value_array[i] = val;
      }
      return new_lut2(templ,
		      value_array,
		      index_array1,
		      index_array2);
    }

  case 3:
    {
      ymuint8 n1;
      s >> n1;
      vector<double> index_array1(n1);
      for (ymuint i = 0; i < n1; ++ i) {
	double val;
	s >> val;
	index_array1[i] = val;
      }

      ymuint8 n2;
      s	>> n2;
      vector<double> index_array2(n2);
      for (ymuint i = 0; i < n2; ++ i) {
	double val;
	s >> val;
	index_array2[i] = val;
      }

      ymuint8 n3;
      s >> n3;
      vector<double> index_array3(n3);
      for (ymuint i = 0; i < n3; ++ i) {
	double val;
	s >> val;
	index_array3[i] = val;
      }

      ymuint n = n1 * n2 * n3;
      vector<double> value_array(n);
      for (ymuint i = 0; i < n; ++ i) {
	double val;
	s >> val;
	value_array[i] = val;
      }

      return new_lut3(templ,
		      value_array,
		      index_array1,
		      index_array2,
		      index_array3);
    }

  default:
    ASSERT_NOT_REACHED;
    break;
  }
  return nullptr;
}

END_NAMESPACE_YM_CELL
