﻿#ifndef CILIBRARY_H
#define CILIBRARY_H

/// @file CiLibrary.h
/// @brief CiLibrary のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmCell/CellLibrary.h"
#include "YmCell/CellArea.h"
#include "YmCell/CellTime.h"
#include "YmCell/CellCapacitance.h"
#include "YmCell/CellResistance.h"
#include "YmCell/CellTiming.h"
#include "YmCell/CellPin.h"
#include "YmUtils/SimpleAlloc.h"
#include "YmUtils/ShString.h"
#include "YmLogic/Expr.h"
#include "CiLutHash.h"
#include "CiCellHash.h"
#include "CiPinHash.h"
#include "CiPatMgr.h"


BEGIN_NAMESPACE_YM_CELL

class CiClass;
class CiGroup;
class CiLutTemplate;
class CiCell;
class CiPin;
class CiTiming;

//////////////////////////////////////////////////////////////////////
/// @class CiLibrary CiLibrary.h "CiLibrary.h"
/// @brief CellLibrary の実装クラス
//////////////////////////////////////////////////////////////////////
class CiLibrary :
  public CellLibrary
{
public:

  /// @brief コンストラクタ
  CiLibrary();

  /// @brief デストラクタ
  virtual
  ~CiLibrary();


public:
  //////////////////////////////////////////////////////////////////////
  // 属性の取得
  //////////////////////////////////////////////////////////////////////

  /// @brief 名前の取得
  virtual
  string
  name() const;

  /// @brief テクノロジの取得
  /// このクラスは常に kTechAsic を返す．
  virtual
  tCellTechnology
  technology() const;

  /// @brief 遅延モデルの取得
  /// 返り値は
  /// - kCellDelayGenericCmos
  /// - kCellDelayPiecewiseCmos
  /// - kCellDelayTableLookup
  /// - kCellDelayCmos2
  /// - kCellDelayDcm
  /// のいずれか
  virtual
  tCellDelayModel
  delay_model() const;

  /// @brief バス命名規則の取得
  virtual
  string
  bus_naming_style() const;

  /// @brief 日付情報の取得
  virtual
  string
  date() const;

  /// @brief リビジョン情報の取得
  virtual
  string
  revision() const;

  /// @brief コメント情報の取得
  virtual
  string
  comment() const;

  /// @brief 時間単位の取得
  virtual
  string
  time_unit() const;

  /// @brief 電圧単位の取得
  virtual
  string
  voltage_unit() const;

  /// @brief 電流単位の取得
  virtual
  string
  current_unit() const;

  /// @brief 抵抗単位の取得
  virtual
  string
  pulling_resistance_unit() const;

  /// @brief 容量単位の取得
  /// @note なぜかここだけインターフェイスが異なる．
  virtual
  double
  capacitive_load_unit() const;

  /// @brief 容量単位文字列の取得
  /// @note なぜかここだけインターフェイスが異なる．
  virtual
  string
  capacitive_load_unit_str() const;

  /// @brief 電力単位の取得
  virtual
  string
  leakage_power_unit() const;

  /// @brief 遅延テーブルのテンプレート数の取得
  virtual
  ymuint
  lu_table_template_num() const;

  /// @brief 遅延テーブルのテンプレートの取得
  /// @param[in] pos 位置番号 ( 0 <= pos < lu_table_template_num() )
  virtual
  const CellLutTemplate*
  lu_table_template(ymuint pos) const;

  /// @brief ルックアップテーブルのテンプレートの取得
  /// @param[in] name テンプレート名
  /// @note なければ nullptr を返す．
  virtual
  const CellLutTemplate*
  lu_table_template(const char* name) const;

  /// @brief バスタイプの取得
  /// @param[in] name バスタイプ名
  /// @note なければ nullptr を返す．
  virtual
  const CellBusType*
  bus_type(const char* name) const;


public:
  //////////////////////////////////////////////////////////////////////
  // セル情報の取得
  //////////////////////////////////////////////////////////////////////

  /// @brief このライブラリの持つセル数の取得
  virtual
  ymuint
  cell_num() const;

  /// @brief セルの取得
  /// @param[in] pos 位置番号( 0 <= pos < cell_num() )
  virtual
  const Cell*
  cell(ymuint pos) const;

  /// @brief 名前からのセルの取得
  virtual
  const Cell*
  cell(const char* name) const;

  /// @brief 名前からのセルの取得
  virtual
  const Cell*
  cell(const string& name) const;

  /// @brief セルグループの個数を返す．
  virtual
  ymuint
  group_num() const;

  /// @brief セルグループを返す．
  /// @param[in] id グループ番号　( 0 <= id < group_num() )
  virtual
  const CellGroup*
  group(ymuint id) const;

  /// @brief NPN同値クラスの個数を返す．
  virtual
  ymuint
  npn_class_num() const;

  /// @brief NPN同値クラスを返す．
  /// @param[in] id クラス番号 ( 0 <= id < npn_class_num() )
  virtual
  const CellClass*
  npn_class(ymuint id) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 論理セルの情報の取得
  //////////////////////////////////////////////////////////////////////

  /// @brief 定数0セルのグループを返す．
  virtual
  const CellGroup*
  const0_func() const;

  /// @brief 定数1セルのグループを返す．
  virtual
  const CellGroup*
  const1_func() const;

  /// @brief バッファセルのグループを返す．
  virtual
  const CellGroup*
  buf_func() const;

  /// @brief インバータセルのグループを返す．
  virtual
  const CellGroup*
  inv_func() const;


public:
  //////////////////////////////////////////////////////////////////////
  // FFセルの情報の取得
  //////////////////////////////////////////////////////////////////////

  /// @brief 単純な型のFFクラスを返す．
  /// @param[in] has_clear クリア端子を持つとき true にする．
  /// @param[in] has_preset プリセット端子を持つとき true にする．
  /// @note 該当するセルがないときでも空のセルクラスが返される．
  virtual
  const CellClass*
  simple_ff_class(bool has_clear,
		  bool has_preset) const;


public:
  //////////////////////////////////////////////////////////////////////
  // ラッチセルの情報の取得
  //////////////////////////////////////////////////////////////////////

  /// @brief 単純な型のラッチクラスを返す．
  /// @param[in] has_clear クリア端子を持つとき true にする．
  /// @param[in] has_preset プリセット端子を持つとき true にする．
  /// @note 該当するセルがないときでも空のセルクラスが返される．
  virtual
  const CellClass*
  simple_latch_class(bool has_clear,
		     bool has_preset) const;


public:
  //////////////////////////////////////////////////////////////////////
  // パタングラフ関係の情報の取得
  //////////////////////////////////////////////////////////////////////

  /// @brief 総パタン数を返す．
  virtual
  ymuint
  pg_pat_num() const;

  /// @brief パタンを返す．
  /// @param[in] id パタン番号 ( 0 <= id < pg_pat_num() )
  virtual
  const CellPatGraph&
  pg_pat(ymuint id) const;

  /// @brief パタンの最大の入力数を得る．
  virtual
  ymuint
  pg_max_input() const;

  /// @brief 総ノード数を返す．
  virtual
  ymuint
  pg_node_num() const;

  /// @brief ノードの種類を返す．
  /// @param[in] id ノード番号 ( 0 <= id < pg_node_num() )
  virtual
  tCellPatType
  pg_node_type(ymuint id) const;

  /// @brief ノードが入力ノードの時に入力番号を返す．
  /// @param[in] id ノード番号 ( 0 <= id < pg_node_num() )
  /// @note 入力ノードでない場合の返り値は不定
  virtual
  ymuint
  pg_input_id(ymuint id) const;

  /// @brief 入力のノード番号を返す．
  /// @param[in] input_id 入力番号 ( 0 <= input_id < pg_input_num() )
  /// @return input_id の入力に対応するノードのノード番号
  virtual
  ymuint
  pg_input_node(ymuint input_id) const;

  /// @brief 総枝数を返す．
  virtual
  ymuint
  pg_edge_num() const;

  /// @brief 枝のファンイン元のノード番号を返す．
  /// @param[in] id 枝番号 ( 0 <= id < pg_edge_num() )
  virtual
  ymuint
  pg_edge_from(ymuint id) const;

  /// @brief 枝のファンアウト先のノード番号を返す．
  /// @param[in] id 枝番号 ( 0 <= id < pg_edge_num() )
  virtual
  ymuint
  pg_edge_to(ymuint id) const;

  /// @brief 枝のファンアウト先の入力位置( 0 or 1 ) を返す．
  /// @param[in] id 枝番号 ( 0 <= id < pg_edge_num() )
  virtual
  ymuint
  pg_edge_pos(ymuint id) const;

  /// @brief 枝の反転属性を返す．
  /// @param[in] id 枝番号 ( 0 <= id < pg_edge_num() )
  virtual
  bool
  pg_edge_inv(ymuint id) const;


public:
  //////////////////////////////////////////////////////////////////////
  // ダンプ/リストア関係の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をバイナリダンプする．
  /// @param[in] s 出力先のストリーム
  virtual
  void
  dump(ODO& s) const;

  /// @brief バイナリダンプされた内容を読み込む．
  /// @param[in] s 入力元のストリーム
  virtual
  void
  restore(IDO& s);


public:
  //////////////////////////////////////////////////////////////////////
  // 情報設定用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 名前を設定する．
  /// @param[in] name 名前
  virtual
  void
  set_name(const string& name);

  /// @brief 'technology' を設定する．
  virtual
  void
  set_technology(tCellTechnology technology);

  /// @brief 遅延モデルを設定する．
  /// @param[in] delay_model 遅延モデル．
  virtual
  void
  set_delay_model(tCellDelayModel delay_model);

  /// @brief 'capacitive_load_unit' を設定する．
  /// @param[in] unit 単位
  /// @param[in] ustr 単位の後に表示する文字列
  virtual
  void
  set_capacitive_load_unit(double unit,
			   const string& ustr);

  /// @brief 属性を設定する(浮動小数点型)
  /// @param[in] attr_name 属性名
  /// @param[in] value 値
  virtual
  void
  set_attr(const string& attr_name,
	   double value);

  /// @brief 属性を設定する(文字列型)．
  /// @param[in] attr_name 属性名
  /// @param[in] value 値
  virtual
  void
  set_attr(const string& attr_name,
	   const string& value);

  /// @brief 遅延テーブルのテンプレート数を設定する．
  virtual
  void
  set_lu_table_template_num(ymuint num);

  /// @brief 1次元の LUT のテンプレートを作る．
  /// @param[in] id ID 番号
  /// @param[in] name 名前
  /// @param[in] var_type1 変数型
  /// @param[in] index_list1 インデックス値のリスト
  virtual
  void
  new_lut_template1(ymuint id,
		    const string& name,
		    tCellVarType var_type1,
		    const vector<double>& index_list1);

  /// @brief 2次元の LUT のテンプレートを作る．
  /// @param[in] id ID 番号
  /// @param[in] name 名前
  /// @param[in] var_type1 変数型
  /// @param[in] index_list1 インデックス値のリスト
  /// @param[in] var_type2 変数型
  /// @param[in] index_list2 インデックス値のリスト
  virtual
  void
  new_lut_template2(ymuint id,
		    const string& name,
		    tCellVarType var_type1,
		    const vector<double>& index_list1,
		    tCellVarType var_type2,
		    const vector<double>& index_list2);

  /// @brief 3次元の LUT のテンプレートを作る．
  /// @param[in] id ID 番号
  /// @param[in] name 名前
  /// @param[in] var_type1 変数型
  /// @param[in] index_list1 インデックス値のリスト
  /// @param[in] var_type2 変数型
  /// @param[in] index_list2 インデックス値のリスト
  /// @param[in] var_type3 変数型
  /// @param[in] index_list3 インデックス値のリスト
  virtual
  void
  new_lut_template3(ymuint id,
		    const string& name,
		    tCellVarType var_type1,
		    const vector<double>& index_list1,
		    tCellVarType var_type2,
		    const vector<double>& index_list2,
		    tCellVarType var_type3,
		    const vector<double>& index_list3);

  /// @brief セル数を設定する．
  /// @param[in] num 設定する値
  virtual
  void
  set_cell_num(ymuint num);

  /// @brief セルを取り出す．
  /// @param[in] pos 位置番号 ( 0 <= pos < cell_num() )
  virtual
  Cell*
  cell(ymuint pos);

  /// @brief 論理セルを生成する．
  /// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
  /// @param[in] name 名前
  /// @param[in] area 面積
  /// @param[in] ni 入力ピン数
  /// @param[in] no 出力ピン数
  /// @param[in] nio 入出力ピン数
  /// @param[in] nb バス数
  /// @param[in] nc バンドル数
  /// @param[in] output_array 出力の情報の配列
  /// @param[in] logic_array 出力の論理式の配列
  /// @param[in] tristated_array トライステート条件の論理式の配列
  /// *1: - false 論理式なし
  ///     - true 論理式あり
  virtual
  void
  new_logic_cell(ymuint cell_id,
		 const string& name,
		 CellArea area,
		 ymuint ni,
		 ymuint no,
		 ymuint nio,
		 ymuint nb,
		 ymuint nc,
		 const vector<bool>& output_array,
		 const vector<Expr>& logic_array,
		 const vector<Expr>& tristate_array);

  /// @brief FFセルを生成する．
  /// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
  /// @param[in] name 名前
  /// @param[in] area 面積
  /// @param[in] ni 入力ピン数
  /// @param[in] no 出力ピン数
  /// @param[in] nio 入出力ピン数
  /// @param[in] nb バス数
  /// @param[in] nc バンドル数
  /// @param[in] output_array 出力の情報の配列(*1)
  /// @param[in] logic_array 出力の論理式の配列
  /// @param[in] tristated_array トライステート条件の論理式の配列
  /// @param[in] next_state "next_state" 関数の式
  /// @param[in] clocked_on "clocked_on" 関数の式
  /// @param[in] clocked_on_also "clocked_on_also" 関数の式
  /// @param[in] clear "clear" 関数の式
  /// @param[in] preset "preset" 関数の式
  /// @param[in] clear_preset_var1 clear と preset が同時にオンになったときの値1
  /// @param[in] clear_preset_var2 clear と preset が同時にオンになったときの値2
  /// *1: - false 論理式なし
  ///     - true 論理式あり
  virtual
  void
  new_ff_cell(ymuint cell_id,
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
	      ymuint clear_preset_var2);

  /// @brief ラッチセルを生成する．
  /// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
  /// @param[in] name 名前
  /// @param[in] area 面積
  /// @param[in] ni 入力ピン数
  /// @param[in] no 出力ピン数
  /// @param[in] nio 入出力ピン数
  /// @param[in] nb バス数
  /// @param[in] nc バンドル数
  /// @param[in] output_array 出力の情報の配列(*1)
  /// @param[in] logic_array 出力の論理式の配列
  /// @param[in] tristated_array トライステート条件の論理式の配列
  /// @param[in] data_in "data_in" 関数の式
  /// @param[in] enable "enable" 関数の式
  /// @param[in] clear "clear" 関数の式
  /// @param[in] preset "preset" 関数の式
  /// @param[in] enable_also "enable_also" 関数の式
  /// @param[in] clear_preset_var1 clear と preset が同時にオンになったときの値1
  /// @param[in] clear_preset_var2 clear と preset が同時にオンになったときの値2
  /// *1: - false 論理式なし
  ///     - true 論理式あり
  virtual
  void
  new_latch_cell(ymuint cell_id,
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
		 ymuint clear_preset_var2);

  /// @brief FSMセルを生成する．
  /// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
  /// @param[in] name 名前
  /// @param[in] area 面積
  /// @param[in] ni 入力ピン数
  /// @param[in] no 出力ピン数
  /// @param[in] nio 入出力ピン数
  /// @param[in] nit 内部ピン数
  /// @param[in] nb バス数
  /// @param[in] nc バンドル数
  /// @param[in] output_array 出力の情報の配列(*1)
  /// @param[in] logic_array 出力の論理式の配列
  /// @param[in] tristated_array トライステート条件の論理式の配列
  /// *1: - false 論理式なし
  ///     - true 論理式あり
  virtual
  void
  new_fsm_cell(ymuint cell_id,
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
	       const vector<Expr>& tristate_array);

  /// @brief セルの入力ピンを生成する．
  /// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
  /// @param[in] pin_id ピン番号 ( 0 <= pin_id < cell->pin_num() )
  /// @param[in] input_id 入力ピン番号 ( 0 <= input_id < cell->input_num2() )
  /// @param[in] name 入力ピン名
  /// @param[in] capacitance 入力ピンの負荷容量
  /// @param[in] rise_capacitance 入力ピンの立ち上がり負荷容量
  /// @param[in] fall_capacitance 入力ピンの立ち下がり負荷容量
  virtual
  void
  new_cell_input(ymuint cell_id,
		 ymuint pin_id,
		 ymuint input_id,
		 const string& name,
		 CellCapacitance capacitance,
		 CellCapacitance rise_capacitance,
		 CellCapacitance fall_capacitance);

  /// @brief セルの出力ピンの内容を設定する．
  /// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
  /// @param[in] pin_id ピン番号 ( 0 <= pin_id < cell->pin_num() )
  /// @param[in] output_id 出力ピン番号 ( 0 <= output_id < cell->output_num2() )
  /// @param[in] name 出力ピン名
  /// @param[in] max_fanout 最大ファンアウト容量
  /// @param[in] min_fanout 最小ファンアウト容量
  /// @param[in] max_capacitance 最大負荷容量
  /// @param[in] min_capacitance 最小負荷容量
  /// @param[in] max_transition 最大遷移時間
  /// @param[in] min_transition 最小遷移時間
  virtual
  void
  new_cell_output(ymuint cell_id,
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
		  CellTime min_transition);

  /// @brief セルの入出力ピンの内容を設定する．
  /// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
  /// @param[in] pin_id 入出力ピン番号 ( 0 <= pin_id < cell->pin_num() )
  /// @param[in] input_id 入力ピン番号 ( 0 <= input_id < cell->input_num2() )
  /// @param[in] output_id 出力ピン番号 ( 0 <= output_id < cell->output_num2() )
  /// @param[in] name 入出力ピン名
  /// @param[in] capacitance 入力ピンの負荷容量
  /// @param[in] rise_capacitance 入力ピンの立ち上がり負荷容量
  /// @param[in] fall_capacitance 入力ピンの立ち下がり負荷容量
  /// @param[in] max_fanout 最大ファンアウト容量
  /// @param[in] min_fanout 最小ファンアウト容量
  /// @param[in] max_capacitance 最大負荷容量
  /// @param[in] min_capacitance 最小負荷容量
  /// @param[in] max_transition 最大遷移時間
  /// @param[in] min_transition 最小遷移時間
  virtual
  void
  new_cell_inout(ymuint cell_id,
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
		 CellTime min_transition);

  /// @brief セルの内部ピンを生成する．
  /// @param[in] cell_id セル番号
  /// @param[in] pin_id ピン番号 ( 0 <= pin_id < cell->pin_num() )
  /// @param[in] int_id 入力ピン番号 ( 0 <= int_id < cell->internal_num() )
  /// @param[in] name 内部ピン名
  void
  new_cell_internal(ymuint cell_id,
		    ymuint pin_id,
		    ymuint int_id,
		    const string& name);

  /// @brief タイミング情報の数を設定する．
  /// @param[in] cell_id セル番号
  /// @param[in] timing_num タイミング情報の数．
  virtual
  void
  set_timing_num(ymuint cell_id,
		 ymuint timing_num);

  /// @brief タイミング情報を作る(ジェネリック遅延モデル)．
  /// @param[in] cell_id セル番号
  /// @param[in] tid タイミングID
  /// @param[in] type タイミングの型
  /// @param[in] cond タイミング条件を表す式
  /// @param[in] intrinsic_rise 立ち上がり固有遅延
  /// @param[in] intrinsic_fall 立ち下がり固有遅延
  /// @param[in] slope_rise 立ち上がりスロープ遅延
  /// @param[in] slope_fall 立ち下がりスロープ遅延
  /// @param[in] rise_resistance 立ち上がり負荷依存係数
  /// @param[in] fall_resistance 立ち下がり負荷依存係数
  virtual
  void
  new_timing_generic(ymuint cell_id,
		     ymuint tid,
		     tCellTimingType type,
		     const Expr& cond,
		     CellTime intrinsic_rise,
		     CellTime intrinsic_fall,
		     CellTime slope_rise,
		     CellTime slope_fall,
		     CellResistance rise_resistance,
		     CellResistance fall_resistance);

  /// @brief タイミング情報を作る(折れ線近似)．
  /// @param[in] cell_id セル番号
  /// @param[in] tid タイミングID
  /// @param[in] timing_type タイミングの型
  /// @param[in] cond タイミング条件を表す式
  /// @param[in] intrinsic_rise 立ち上がり固有遅延
  /// @param[in] intrinsic_fall 立ち下がり固有遅延
  /// @param[in] slope_rise 立ち上がりスロープ遅延
  /// @param[in] slope_fall 立ち下がりスロープ遅延
  virtual
  void
  new_timing_piecewise(ymuint cell_id,
		       ymuint tid,
		       tCellTimingType timing_type,
		       const Expr& cond,
		       CellTime intrinsic_rise,
		       CellTime intrinsic_fall,
		       CellTime slope_rise,
		       CellTime slope_fall,
		       CellResistance rise_pin_resistance,
		       CellResistance fall_pin_resistance);

  /// @brief タイミング情報を作る(非線形タイプ1)．
  /// @param[in] cell_id セル番号
  /// @param[in] tid タイミングID
  /// @param[in] cell_id セル番号 ( 0 <= cell_id < cell_num() )
  /// @param[in] opin_id 出力(入出力)ピン番号 ( *1 )
  /// @param[in] ipin_id 関連する入力(入出力)ピン番号 ( *2 )
  /// @param[in] timing_type タイミングの型
  /// @param[in] timing_sense タイミングセンス
  /// @param[in] cond タイミング条件を表す式
  /// @param[in] cell_rise 立ち上がりセル遅延テーブル
  /// @param[in] cell_fall 立ち下がりセル遅延テーブル
  virtual
  void
  new_timing_lut1(ymuint cell_id,
		  ymuint tid,
		  tCellTimingType timing_type,
		  const Expr& cond,
		  CellLut* cell_rise,
		  CellLut* cell_fall,
		  CellLut* rise_transition,
		  CellLut* fall_transition);

  /// @brief タイミング情報を作る(非線形タイプ2)．
  /// @param[in] cell_id セル番号
  /// @param[in] tid タイミングID
  /// @param[in] timing_type タイミングの型
  /// @param[in] cond タイミング条件を表す式
  /// @param[in] rise_transition 立ち上がり遷移遅延テーブル
  /// @param[in] fall_transition 立ち下がり遷移遅延テーブル
  /// @param[in] rise_propagation 立ち上がり伝搬遅延テーブル
  /// @param[in] fall_propagation 立ち下がり伝搬遅延テーブル
  virtual
  void
  new_timing_lut2(ymuint cell_id,
		  ymuint tid,
		  tCellTimingType timing_type,
		  const Expr& cond,
		  CellLut* rise_transition,
		  CellLut* fall_transition,
		  CellLut* rise_propagation,
		  CellLut* fall_propagation);

  /// @brief タイミング情報をセットする．
  /// @param[in] cell_id セルID
  /// @param[in] input_id 入力ピンID
  /// @param[in] output_id 出力ピンID
  /// @param[in] timing_sense タイミングセンス
  /// @param[in] tid_list タイミングIDのリスト
  virtual
  void
  set_timing(ymuint cell_id,
	     ymuint input_id,
	     ymuint output_id,
	     tCellTimingSense timing_sense,
	     const vector<ymuint>& tid_list);

  /// @brief 1次元の LUT を作る．
  /// @param[in] lut_template テンプレート
  /// @param[in] value_array 値の配列
  /// @param[in] index_array インデックス値のリスト
  virtual
  CellLut*
  new_lut1(const CellLutTemplate* lut_template,
	   const vector<double>& value_array,
	   const vector<double>& index_array = vector<double>());

  /// @brief 2次元の LUT を作る．
  /// @param[in] lut_template テンプレート
  /// @param[in] value_array 値の配列
  /// @param[in] index_array1 インデックス値のリスト
  /// @param[in] index_array2 インデックス値のリスト
  virtual
  CellLut*
  new_lut2(const CellLutTemplate* lut_template,
	   const vector<double>& value_array,
	   const vector<double>& index_array1 = vector<double>(),
	   const vector<double>& index_array2 = vector<double>());

  /// @brief 3次元の LUT を作る．
  /// @param[in] lut_template テンプレート
  /// @param[in] value_array 値の配列
  /// @param[in] index_array1 インデックス値のリスト
  /// @param[in] index_array2 インデックス値のリスト
  /// @param[in] index_array3 インデックス値のリスト
  virtual
  CellLut*
  new_lut3(const CellLutTemplate* lut_template,
	   const vector<double>& value_array,
	   const vector<double>& index_array1 = vector<double>(),
	   const vector<double>& index_array2 = vector<double>(),
	   const vector<double>& index_array3 = vector<double>());

  /// @brief セルのグループ分けを行う．
  /// @note 論理セルのパタングラフも作成する．
  virtual
  void
  compile();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief クラス数を設定する．
  /// @param[in] nc クラス数
  /// @note 同時にクラスの配列の確保を行う．
  void
  set_class_num(ymuint nc);

  /// @brief グループ数を設定する．
  /// @param[in] ng グループ数
  /// @note 同時にグループの配列の確保を行う．
  void
  set_group_num(ymuint ng);

  /// @brief セルを追加する．
  /// @param[in] id セル番号
  /// @param[in] cell セル
  void
  add_cell(ymuint id,
	   CiCell* cell);

  /// @brief LUT テンプレートを読み込む．
  void
  restore_lut_template(IDO& s,
		       ymuint id);

  /// @brief LUT を読み込む．
  CellLut*
  restore_lut(IDO& s);


public:
  //////////////////////////////////////////////////////////////////////
  // ピンハッシュ用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ピンの登録
  /// @param[in] pin 登録するピン
  void
  add_pin(CiPin* pin);

  /// @brief ピン名からピンを取り出す．
  /// @param[in] cell セル
  /// @param[in] name ピン名
  CiPin*
  get_pin(const CiCell* cell,
	  ShString name);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // メモリ確保用のオブジェクト
  SimpleAlloc mAlloc;

  // 名前
  string mName;

  // テクノロジ
  tCellTechnology mTechnology;

  // バス命名規則
  string mBusNamingStyle;

  // 日付
  string mDate;

  // リビジョン
  string mRevision;

  // コメント
  string mComment;

  // 時間単位
  string mTimeUnit;

  // 電圧単位
  string mVoltageUnit;

  // 電流単位
  string mCurrentUnit;

  // 抵抗単位
  string mPullingResistanceUnit;

  // 容量単位
  double mCapacitiveLoadUnit;

  // 容量単位の文字列
  string mCapacitiveLoadUnitStr;

  // 電力単位
  string mLeakagePowerUnit;

  // 遅延モデル
  tCellDelayModel mDelayModel;

  // 遅延テンプレート数
  ymuint32 mLutTemplateNum;

  // 遅延テンプレートの配列
  CiLutTemplate** mLutTemplateArray;

  // 遅延テンプレートのハッシュ表
  CiLutHash mLutHash;

  // セル数
  ymuint32 mCellNum;

  // セルのポインタの配列
  CiCell** mCellArray;

  // 名前をキーにしたセルのハッシュ表
  CiCellHash mCellHash;

  // ピン名をキーにしたピンのハッシュ表
  CiPinHash mPinHash;

  // セルグループ数
  ymuint32 mGroupNum;

  // セルグループの配列
  CiGroup* mGroupArray;

  // NPN同値クラスの数
  ymuint32 mClassNum;

  // NPN同値クラスの配列
  CiClass* mClassArray;

  // 論理セルグループの情報
  const CellGroup* mLogicGroup[4];

  // FFクラスの情報
  const CellClass* mFFClass[4];

  // ラッチクラスの情報
  const CellClass* mLatchClass[4];

  // パタングラフを管理するクラス
  CiPatMgr mPatMgr;

};

END_NAMESPACE_YM_CELL

#endif // CILIBRARY_H
