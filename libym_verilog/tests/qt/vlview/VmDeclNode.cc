﻿
/// @file VmDeclNode.cc
/// @brief VmDeclNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "VmDeclNode.h"
#include "VmMiscNode.h"
#include "YmVerilog/vl/VlModule.h"
#include "YmVerilog/vl/VlUdp.h"
#include "YmVerilog/vl/VlTaskFunc.h"
#include "YmVerilog/vl/VlIODecl.h"
#include "YmVerilog/vl/VlDecl.h"
#include "YmVerilog/vl/VlDeclArray.h"
#include "YmVerilog/vl/VlRange.h"


BEGIN_NAMESPACE_YM_VERILOG

//////////////////////////////////////////////////////////////////////
// クラス VmNode
//////////////////////////////////////////////////////////////////////

// @brief IODeclListNode を追加する．
// @param[in] iodecl_list IO宣言のリスト
void
VmNode::add_iolist(const vector<const VlIODecl*>& iodecl_list) const
{
  add_child( new VmIODeclListNode(iodecl_list) );
}

// @brief DeclListNode を追加する．
// @param[in] label ラベル
// @param[in] decl_list 宣言のリスト
void
VmNode1::add_decllist(const QString& label,
		      const vector<const VlDecl*>& decl_list) const
{
  add_child( new VmDeclListNode(label, decl_list) );
}

// @brief DeclArrayListNode を追加する．
// @param[in] label ラベル
// @param[in] decl_list 宣言のリスト
void
VmNode1::add_declarraylist(const QString& label,
			   const vector<const VlDeclArray*>& decl_list) const
{
  add_child( new VmDeclArrayListNode(label, decl_list) );
}


//////////////////////////////////////////////////////////////////////
// クラス VmIODeclListNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] io_array IO宣言の配列
VmIODeclListNode::VmIODeclListNode(const vector<const VlIODecl*>& io_array) :
  mIOArray(io_array)
{
}

// @brief デストラクタ
VmIODeclListNode::~VmIODeclListNode()
{
}

// @brief データを返す．
// @param[in] column コラム番号
// @param[in] role
QVariant
VmIODeclListNode::data(int column,
		       int role) const
{
  if ( role == Qt::DisplayRole ) {
    if ( column == 0 ) {
      return "vpiIODecl list";
    }
    else if ( column == 1 ) {
      return "";
    }
  }
  return QVariant();
}

// @brief 対象のファイル上での位置を返す．
FileRegion
VmIODeclListNode::loc() const
{
  if ( mIOArray.empty() ) {
    // 多分ないと思うけど
    return FileRegion();
  }
  else {
    FileRegion first = mIOArray.front()->file_region();
    FileRegion last = mIOArray.back()->file_region();
    return FileRegion(first, last);
  }
}

// @brief 子供の配列を作る．
void
VmIODeclListNode::expand() const
{
  for (vector<const VlIODecl*>::const_iterator p = mIOArray.begin();
       p != mIOArray.end(); ++ p) {
    const VlIODecl* io = *p;
    add_child( new VmIODeclNode(io) );
  }
}


//////////////////////////////////////////////////////////////////////
// クラス VmIODeclNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] io IO宣言要素
VmIODeclNode::VmIODeclNode(const VlIODecl* io) :
  mIODecl(io)
{
}

// @brief デストラクタ
VmIODeclNode::~VmIODeclNode()
{
}

// @brief データを返す．
// @param[in] column コラム番号
// @param[in] role
QVariant
VmIODeclNode::data(int column,
		   int role) const
{
  if ( role == Qt::DisplayRole ) {
    if ( column == 0 ) {
      return "vpiIODecl";
    }
    else if ( column == 1 ) {
      return mIODecl->name();
    }
  }
  return QVariant();
}

// @brief 対象のファイル上での位置を返す．
FileRegion
VmIODeclNode::loc() const
{
  return mIODecl->file_region();
}

// @brief 子供の配列を作る．
void
VmIODeclNode::expand() const
{
  add_dir(mIODecl->direction());
  add_bool("vpiSigned", mIODecl->is_signed());
#if 0
  add_bool("vpiScalar", mIODecl->is_scalar());
  add_bool("vpiVector", mIODecl->is_vector());
#endif
  add_int("vpiSize", mIODecl->bit_size());

  const VlModule* module = mIODecl->module();
  if ( module ) {
    add_str("vpiModule", module->full_name());
  }
  const VlUdpDefn* udp = mIODecl->udp_defn();
  if ( udp ) {
    add_str("vpiUdpDefn", udp->def_name());
  }
  const VlTaskFunc* task = mIODecl->task();
  if ( task ) {
    add_str("vpiTask", task->full_name());
  }
  const VlTaskFunc* func = mIODecl->function();
  if ( func ) {
    add_str("vpiFunction", func->full_name());
  }
  if ( mIODecl->has_range() ) {
    add_str("vpiLeftRange", mIODecl->left_range_string());
    add_str("vpiRightRange", mIODecl->right_range_string());
    add_int("vpiLeftRangeVal", mIODecl->left_range_val());
    add_int("vpiRightRangeVal", mIODecl->right_range_val());
  }
}


//////////////////////////////////////////////////////////////////////
// クラス VmDeclListNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] label ラベル
// @param[in] decl_array 宣言要素の配列
VmDeclListNode::VmDeclListNode(const QString& label,
			       const vector<const VlDecl*>& decl_array) :
  mLabel(label),
  mDeclArray(decl_array)
{
}

// @brief デストラクタ
VmDeclListNode::~VmDeclListNode()
{
}

// @brief データを返す．
// @param[in] column コラム番号
// @param[in] role
QVariant
VmDeclListNode::data(int column,
		     int role) const
{
  if ( role == Qt::DisplayRole ) {
    if ( column == 0 ) {
      return mLabel + " list";
    }
    else if ( column == 1 ) {
      return "";
    }
  }
  return QVariant();
}

// @brief 対象のファイル上での位置を返す．
FileRegion
VmDeclListNode::loc() const
{
  if ( mDeclArray.empty() ) {
    return FileRegion();
  }
  else {
    FileRegion first = mDeclArray.front()->file_region();
    FileRegion last = mDeclArray.back()->file_region();
    return FileRegion(first, last);
  }
}

// @brief 子供の配列を作る．
void
VmDeclListNode::expand() const
{
  for (vector<const VlDecl*>::const_iterator p = mDeclArray.begin();
       p != mDeclArray.end(); ++ p) {
    add_child( new VmDeclNode(mLabel, *p) );
  }
}


//////////////////////////////////////////////////////////////////////
// クラス VmDeclNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] label ラベル
// @param[in] declitem 宣言要素
VmDeclNode::VmDeclNode(const QString& label,
		       const VlDecl* decl) :
  mLabel(label),
  mDecl(decl)
{
}

// @brief デストラクタ
VmDeclNode::~VmDeclNode()
{
}

// @brief データを返す．
// @param[in] column コラム番号
// @param[in] role
QVariant
VmDeclNode::data(int column,
		 int role) const
{
  if ( role == Qt::DisplayRole ) {
    if ( column == 0 ) {
      return mLabel;
    }
    else if ( column == 1 ) {
      return mDecl->name();
    }
  }
  return QVariant();
}

// @brief 対象のファイル上での位置を返す．
FileRegion
VmDeclNode::loc() const
{
  return mDecl->file_region();
}

// @brief 子供の配列を作る．
void
VmDeclNode::expand() const
{
  const char* nm = nullptr;
  switch ( mDecl->type() ) {
  case kVpiNet:             nm = "vpiNet"; break;
  case kVpiReg:             nm = "vpiReg"; break;
  case kVpiIntegerVar:      nm = "vpiIntegerVar"; break;
  case kVpiRealVar:         nm = "vpiRealVar"; break;
  case kVpiTimeVar:         nm = "vpiTimeVar"; break;
  case kVpiVarSelect:       nm = "vpiVarSelect"; break;
  case kVpiNamedEvent:      nm = "vpiNamedEvent"; break;
  case kVpiParameter:       nm = "vpiParameter"; break;
  case kVpiSpecParam:       nm = "vpiSpecParam"; break;
  default: assert_not_reached( __FILE__, __LINE__ );
  }
  add_str("vpiType", nm);
  add_str("vpiFullName", mDecl->full_name());
#if 0
  add_bool("vpiExpanded", mDecl->is_expanded());
  add_bool("vpiImplicitDecl", mDecl->is_implicit_decl());
  add_bool("vpiNetDeclAssign", mDecl->has_net_decl_assign());
  add_bool("vpiScalar", mDecl->is_scalar());
  add_bool("vpiVector", mDecl->is_vector());
  add_bool("vpiExplicitScalar", mDecl->is_explicit_scalar());
  add_bool("vpiExplicitVector", mDecl->is_explicit_vector());
#endif
  add_child( new VmVsNode(mDecl->vs_type()) );
  add_bool("vpiSigned", mDecl->is_signed());
  add_int("vpiSize", mDecl->bit_size());
  add_str("vpiModule", mDecl->parent_module()->full_name());
  add_str("vpiScope", mDecl->parent()->full_name());
  // 親の配列

  if ( mDecl->type() == kVpiNet ) {
    add_strength("vpiStrength0", mDecl->drive0());
    add_strength("vpiStrength1", mDecl->drive1());
    add_strength("vpiChargeStrength", mDecl->charge());
    add_delay(mDecl->delay());
  }

#if 0
  add_child("vpiIndex", mDecl->index());

  if ( mDecl->port_inst() ) {
    add_expr("vpiPortInst", mDecl->port_inst());
  }
  if ( mDecl->ports() ) {
    add_expr("vpiPorts", mDecl->ports());
  }
#endif

  if ( mDecl->has_range() ) {
    add_str("vpiLeftRange", mDecl->left_range_string());
    add_str("vpiRightRange", mDecl->right_range_string());
    add_int("vpiLeftRangeVal", mDecl->left_range_val());
    add_int("vpiRightRangeVal", mDecl->right_range_val());
  }
}


//////////////////////////////////////////////////////////////////////
// クラス VmDeclArrayListNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] label ラベル
// @param[in] decl_array 宣言要素の配列
VmDeclArrayListNode::VmDeclArrayListNode(const QString& label,
					 const vector<const VlDeclArray*>& decl_array) :
  mLabel(label),
  mDeclArray(decl_array)
{
}

// @brief デストラクタ
VmDeclArrayListNode::~VmDeclArrayListNode()
{
}

// @brief データを返す．
// @param[in] column コラム番号
// @param[in] role
QVariant
VmDeclArrayListNode::data(int column,
			  int role) const
{
  if ( role == Qt::DisplayRole ) {
    if ( column == 0 ) {
      return mLabel + " list";
    }
    else if ( column == 1 ) {
      return "";
    }
  }
  return QVariant();
}

// @brief 対象のファイル上での位置を返す．
FileRegion
VmDeclArrayListNode::loc() const
{
  if ( mDeclArray.empty() ) {
    return FileRegion();
  }
  else {
    FileRegion first = mDeclArray.front()->file_region();
    FileRegion last = mDeclArray.back()->file_region();
    return FileRegion(first, last);
  }
}

// @brief 子供の配列を作る．
void
VmDeclArrayListNode::expand() const
{
  for (vector<const VlDeclArray*>::const_iterator p = mDeclArray.begin();
       p != mDeclArray.end(); ++ p) {
    add_child( new VmDeclArrayNode(mLabel, *p) );
  }
}


//////////////////////////////////////////////////////////////////////
// クラス VmDeclArrayNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] label ラベル
// @param[in] declitem 宣言要素
VmDeclArrayNode::VmDeclArrayNode(const QString& label,
				 const VlDeclArray* decl) :
  mLabel(label),
  mDecl(decl)
{
}

// @brief デストラクタ
VmDeclArrayNode::~VmDeclArrayNode()
{
}

// @brief データを返す．
// @param[in] column コラム番号
// @param[in] role
QVariant
VmDeclArrayNode::data(int column,
		      int role) const
{
  if ( role == Qt::DisplayRole ) {
    if ( column == 0 ) {
      return mLabel;
    }
    else if ( column == 1 ) {
      return mDecl->name();
    }
  }
  return QVariant();
}

// @brief 対象のファイル上での位置を返す．
FileRegion
VmDeclArrayNode::loc() const
{
  return mDecl->file_region();
}

// @brief 子供の配列を作る．
void
VmDeclArrayNode::expand() const
{
  const char* nm = nullptr;
  switch ( mDecl->type() ) {
  case kVpiNetArray:        nm = "vpiNetArray"; break;
  case kVpiRegArray:        nm = "vpiRegArray"; break;
  case kVpiIntegerVar:      nm = "vpiIntegerVar"; break;
  case kVpiRealVar:         nm = "vpiRealVar"; break;
  case kVpiTimeVar:         nm = "vpiTimeVar"; break;
  case kVpiNamedEventArray: nm = "vpiNamedEventArray"; break;
  default: assert_not_reached( __FILE__, __LINE__ );
  }
  add_str("vpiType", nm);
  add_str("vpiFullName", mDecl->full_name());
#if 0
  add_bool("vpiExpanded", mDecl->is_expanded());
  add_bool("vpiImplicitDecl", mDecl->is_implicit_decl());
  add_bool("vpiNetDeclAssign", mDecl->has_net_decl_assign());
  add_bool("vpiScalar", mDecl->is_scalar());
  add_bool("vpiVector", mDecl->is_vector());
  add_bool("vpiExplicitScalar", mDecl->is_explicit_scalar());
  add_bool("vpiExplicitVector", mDecl->is_explicit_vector());
#endif
  add_child( new VmVsNode(mDecl->vs_type()) );
  add_bool("vpiSigned", mDecl->is_signed());
  add_int("vpiSize", mDecl->bit_size());
  add_str("vpiModule", mDecl->parent_module()->full_name());
  add_str("vpiScope", mDecl->parent()->full_name());
  add_bool("vpiMultiArray", mDecl->is_multi_array());
  // 親の配列

  ymuint n = mDecl->dimension();
  for (ymuint i = 0; i < n; ++ i) {
    add_child( new VmRangeNode(mDecl->range(i)) );
  }

  if ( mDecl->type() == kVpiNetArray ) {
    add_strength("vpiStrength0", mDecl->drive0());
    add_strength("vpiStrength1", mDecl->drive1());
    add_strength("vpiChargeStrength", mDecl->charge());
    add_delay(mDecl->delay());
  }

#if 0
  add_child("vpiIndex", mDecl->index());

  if ( mDecl->port_inst() ) {
    add_expr("vpiPortInst", mDecl->port_inst());
  }
  if ( mDecl->ports() ) {
    add_expr("vpiPorts", mDecl->ports());
  }
#endif

  if ( mDecl->has_range() ) {
    add_str("vpiLeftRange", mDecl->left_range_string());
    add_str("vpiRightRange", mDecl->right_range_string());
    add_int("vpiLeftRangeVal", mDecl->left_range_val());
    add_int("vpiRightRangeVal", mDecl->right_range_val());
  }
}


//////////////////////////////////////////////////////////////////////
// クラス VmRangeNode
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] range 範囲
VmRangeNode::VmRangeNode(const VlRange* range) :
  mRange(range)
{
}

// @brief デストラクタ
VmRangeNode::~VmRangeNode()
{
}

// @brief データを返す．
// @param[in] column コラム番号
// @param[in] role
QVariant
VmRangeNode::data(int column,
		  int role) const
{
  if ( role == Qt::DisplayRole ) {
    if ( column == 0 ) {
      return "vpiRange";
    }
    else if ( column == 1 ) {
      return "";
    }
  }
  return QVariant();
}

// @brief 対象のファイル上での位置を返す．
FileRegion
VmRangeNode::loc() const
{
  return mRange->file_region();
}

// @brief 子供の配列を作る．
void
VmRangeNode::expand() const
{
  add_str("vpiLeftRange", mRange->left_range_string());
  add_str("vpiRightRange", mRange->right_range_string());
  add_int("vpiLeftRangeVal", mRange->left_range_val());
  add_int("vpiRightRangeVal", mRange->right_range_val());
}

END_NAMESPACE_YM_VERILOG
