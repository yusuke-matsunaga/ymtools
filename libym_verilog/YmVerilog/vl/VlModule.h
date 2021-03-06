﻿#ifndef VERILOG_VL_VLMODULE_H
#define VERILOG_VL_VLMODULE_H

/// @file YmVerilog/vl/VlModule.h
/// @brief VlModule のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmVerilog/vl/VlNamedObj.h"
#include "YmVerilog/vl/VlFwd.h"


BEGIN_NAMESPACE_YM_VERILOG

//////////////////////////////////////////////////////////////////////
/// @class VlModuleArray VlModule.h "YmVerilog/vl/VlModule.h"
/// @brief elaboration 中の module 配列を表すクラス
//////////////////////////////////////////////////////////////////////
class VlModuleArray :
  public VlNamedObj
{
protected:

  /// @brief デストラクタ
  virtual
  ~VlModuleArray() { }


public:
  //////////////////////////////////////////////////////////////////////
  // VlModuleArray の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 範囲の MSB の値を返す．
  virtual
  int
  left_range_val() const = 0;

  /// @brief 範囲の LSB の値を返す．
  virtual
  int
  right_range_val() const = 0;

  /// @brief 範囲のMSBを表す文字列の取得
  virtual
  string
  left_range_string() const = 0;

  /// @brief 範囲のLSBを表す文字列の取得
  virtual
  string
  right_range_string() const = 0;

  /// @brief 要素数を返す．
  virtual
  ymuint
  elem_num() const = 0;

  /// @brief 要素を返す．
  /// @param[in] offset 位置番号 (0 <= offset < elem_num())
  virtual
  const VlModule*
  elem_by_offset(ymuint offset) const = 0;

  /// @brief 要素を返す．
  /// @param[in] index インデックス
  virtual
  const VlModule*
  elem_by_index(int index) const = 0;

};


//////////////////////////////////////////////////////////////////////
/// @class VlModule VlModule.h "YmVerilog/vl/VlModule.h"
/// @brief elaboration 中の module を表すクラス
//////////////////////////////////////////////////////////////////////
class VlModule :
  public VlNamedObj
{
protected:

  /// @brief デストラクタ
  virtual
  ~VlModule() { }


public:
  //////////////////////////////////////////////////////////////////////
  // VlNamedObj の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 自分の属する module を得る．
  /// @note このクラスでは this を返す．
  virtual
  const VlModule*
  parent_module() const ;


public:
  //////////////////////////////////////////////////////////////////////
  // VlModule の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief definition location を返す．
  /// @return 定義側のファイル位置の情報を返す．
  virtual
  FileRegion
  def_file_region() const = 0;

  /// @brief definition name を返す．
  /// @return 定義名を返す．
  virtual
  const char*
  def_name() const = 0;

  /// @brief cell instance のチェック
  /// @return cell instance の場合に true を返す．
  virtual
  bool
  is_cell_instance() const = 0;

  /// @brief protect のチェック
  /// @return protect されていたら true を返す．
  virtual
  bool
  is_protected() const = 0;

  /// @brief top module の時 true を返す．
  virtual
  bool
  is_top_module() const = 0;

  /// @brief time unit を返す．
  /// @return 結果は 2 〜 -15 の整数
  /// @return もしくは未定義を表す -16
  virtual
  int
  time_unit() const = 0;

  /// @brief time precision を返す．
  /// @return 結果は 2 〜 -15 の整数
  /// @return もしくは未定義を表す -16
  virtual
  int
  time_precision() const = 0;

  /// @brief default net type を返す．
  virtual
  tVpiNetType
  def_net_type() const = 0;

  /// @brief unconnected drive を返す．
  virtual
  tVpiUnconnDrive
  unconn_drive() const = 0;

  /// @brief default delay mode を返す．
  virtual
  tVpiDefDelayMode
  def_delay_mode() const = 0;

  /// @brief default decay time を返す．
  virtual
  int
  def_decay_time() const = 0;

  /// @brief config 情報を返す．
  virtual
  string
  config() const = 0;

  /// @brief library 情報を返す．
  virtual
  string
  library() const = 0;

  /// @brief cell 情報を返す．
  virtual
  string
  cell() const = 0;

  /// @brief 配列要素の時 true を返す．
  virtual
  bool
  is_array() const = 0;

  /// @brief インデックスの値を返す．
  /// @note 配列要素の時のみ意味を持つ．
  virtual
  int
  index() const = 0;

  /// @brief 親の配列を返す．
  /// @note 配列要素の時のみ意味を持つ．
  virtual
  const VlModuleArray*
  module_array() const = 0;

  /// @brief ポート数を返す．
  virtual
  ymuint
  port_num() const = 0;

  /// @brief ポートの取得
  /// @param[in] pos 取得するポートの位置 (0 <= pos < port_num())
  /// @return pos 番目のポートを返す．
  virtual
  const VlPort*
  port(ymuint pos) const = 0;

  /// @brief 入出力数を得る．
  virtual
  ymuint
  io_num() const = 0;

  /// @brief 入出力の取得
  /// @param[in] pos 位置番号 ( 0 <= pos < io_num() )
  virtual
  const VlIODecl*
  io(ymuint pos) const = 0;

};

END_NAMESPACE_YM_VERILOG

#endif // VERILOG_VL_VLMODULE_H
