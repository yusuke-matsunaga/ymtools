﻿
/// @file ReaderImpl.cc
/// @brief ReaderImpl の実装クラス
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "ReaderImpl.h"
#include "DeclMap.h"
#include "Driver.h"
#include "Env.h"
#include "YmNetworks/MvnMgr.h"
#include "YmNetworks/MvnModule.h"
#include "YmNetworks/MvnPort.h"
#include "YmNetworks/MvnNode.h"
#include "YmVerilog/BitVector.h"
#include "YmVerilog/vl/VlModule.h"
#include "YmVerilog/vl/VlPrimitive.h"
#include "YmVerilog/vl/VlUdp.h"
#include "YmVerilog/vl/VlIODecl.h"
#include "YmVerilog/vl/VlDecl.h"
#include "YmVerilog/vl/VlDeclArray.h"
#include "YmVerilog/vl/VlPort.h"
#include "YmVerilog/vl/VlContAssign.h"
#include "YmVerilog/vl/VlProcess.h"
#include "YmVerilog/vl/VlStmt.h"
#include "YmVerilog/vl/VlControl.h"
#include "YmVerilog/vl/VlExpr.h"
#include "YmVerilog/vl/VlRange.h"

#include "YmUtils/MsgMgr.h"


BEGIN_NAMESPACE_YM_NETWORKS_MVN_VERILOG


BEGIN_NONAMESPACE

const
bool debug_driver = false;

END_NONAMESPACE


// @brief コンストラクタ
ReaderImpl::ReaderImpl() :
  mGlobalEnv(mDeclHash)
{
  mMvnMgr = nullptr;
}

// @brief デストラクタ
ReaderImpl::~ReaderImpl()
{
}

// @brief 内部のデータをクリアする．
void
ReaderImpl::clear()
{
  mVlMgr.clear();
}

// @brief verilog 形式のファイルを読み込む．
// @param[in] filename ファイル名
// @param[in] searchpath サーチパス
// @param[in] watcher_list 行番号ウォッチャーのリスト
// @retval true 正常に読み込めた．
// @retval false 読込中にエラーが起こった．
bool
ReaderImpl::read(const string& filename,
		 const SearchPathList& searchpath,
		 const list<VlLineWatcher*> watcher_list)
{
  return mVlMgr.read_file(filename, searchpath, watcher_list);
}

// @brief 今まで読み込んだ情報からネットワークを生成する．
// @param[in] mgr ネットワーク生成用のマネージャ
// @retval true 正常に処理を行った．
// @retval false 生成中にエラーが起こった．
bool
ReaderImpl::gen_network(MvnMgr& mgr,
			const CellLibrary* cell_library,
			MvnVlMap& node_map)
{
  if ( MsgMgr::error_num() > 0 ) {
    return false;
  }

  mVlMgr.elaborate(cell_library);

  if ( MsgMgr::error_num() > 0 ) {
    return false;
  }

  mMvnMgr = &mgr;

  mIODeclMap.clear();
  mDeclHash.clear();
  mGlobalEnv.clear();
  mNodeMap.clear();
  mDriverList.clear();

  MvnModule* module0 = nullptr;
  list<const VlModule*> tmp_list(mVlMgr.topmodule_list());
  for (list<const VlModule*>::const_iterator p = tmp_list.begin();
       p != tmp_list.end(); ++ p) {
    const VlModule* vl_module = *p;

    if ( vl_module->is_cell_instance() ) continue;

    // module を実体化
    MvnModule* module = gen_module(vl_module);
    if ( module == nullptr ) {
      return false;
    }
    if ( module0 == nullptr ) {
      module0 = module;
    }
    else {
      MsgMgr::put_msg(__FILE__, __LINE__,
		      vl_module->file_region(),
		      kMsgError,
		      "MVN_VL",
		      "More than one top modules.");
      return false;
    }
  }

  // 結線を行う．
  ymuint n = mMvnMgr->max_node_id();
  for (ymuint i = 0; i < n; ++ i) {
    MvnNode* node = mMvnMgr->_node(i);
    if ( node == nullptr ) continue;
    const vector<Driver>& dlist = driver_list(node);
    if ( dlist.empty() ) continue;
    ymuint bw = node->input(0)->bit_width();
    vector<Driver> tmp(bw);
    for (vector<Driver>::const_iterator p = dlist.begin();
	 p != dlist.end(); ++ p) {
      const Driver& driver = *p;
      if ( driver.is_simple() ) {
	for (ymuint i = 0; i < bw; ++ i) {
	  if ( tmp[i].rhs_node() != nullptr ) {
	    error_drivers(node, tmp[i], driver);
	  }
	  tmp[i] = driver;
	}
      }
      else if ( driver.has_bitselect() ) {
	ymuint index = driver.index();
	if ( tmp[index].rhs_node() != nullptr ) {
	  error_drivers(node, tmp[index], driver);
	}
	tmp[index] = driver;
      }
      else {
	ymuint msb = driver.msb();
	ymuint lsb = driver.lsb();
	for (ymuint i = lsb; i <= msb; ++ i) {
	  if ( tmp[i].rhs_node() != nullptr ) {
	    error_drivers(node, tmp[i], driver);
	  }
	  tmp[i] = driver;
	}
      }
    }

    // 明示的なドライバがない場合の処理
    for (ymuint i = 0; i < bw; ++ i) {
      if ( tmp[i].rhs_node() == nullptr ) {
	//MsgMgr::put_msg(__FILE__, __LINE__,
#warning "TODO: warning メッセージを出すようにする．"
	vector<ymuint32> val(1, 0);
	MvnNode* ud_node = mMvnMgr->new_const(module0, 1, val);
	if ( bw == 1 ) {
	  tmp[i] = Driver(FileRegion(), ud_node);
	}
	else {
	  tmp[i] = Driver(FileRegion(), ud_node, i);
	}
      }
    }

    Driver prev;
    vector<Driver> tmp2;
    tmp2.reserve(bw);
    for (ymuint i = 0; i < bw; ++ i) {
      ymuint idx = bw - i - 1;
      const Driver& driver = tmp[idx];
      if ( driver != prev ) {
	tmp2.push_back(driver);
	prev = driver;
	ASSERT_COND( driver.rhs_node() != nullptr );
      }
    }

    ymuint nd = tmp2.size();
    if ( nd == 1 ) {
      const Driver& driver = tmp2[0];
      MvnNode* src_node = driver.rhs_node();
      mMvnMgr->connect(src_node, 0, node, 0);
    }
    else {
      vector<ymuint> bw_array(nd);
      for (ymuint i = 0; i < nd; ++ i) {
	const Driver& driver = tmp2[i];
	ASSERT_COND( driver.rhs_node() != nullptr );
	if ( driver.has_bitselect() ) {
	  bw_array[i] = 1;
	}
	else if ( driver.has_partselect() ) {
	  bw_array[i] = driver.msb() - driver.lsb() + 1;
	}
	else {
	  ASSERT_NOT_REACHED;
	}
      }
      MvnNode* node1 = mMvnMgr->new_concat(module0, bw_array);
      mMvnMgr->connect(node1, 0, node, 0);
      for (ymuint i = 0; i < nd; ++ i) {
	const Driver& driver = tmp2[i];
	mMvnMgr->connect(driver.rhs_node(), 0, node1, i);
      }
    }
  }

  // 冗長な through ノードを取り除く
  {
    for (ymuint i = 0; i < n; ++ i) {
      MvnNode* node = mMvnMgr->_node(i);
      if ( node != nullptr && node->type() == MvnNode::kThrough ) {
	const MvnNode* src_node = node->input(0)->src_node();
	if ( src_node!= nullptr ) {
	  mNodeMap.copy(node->id(), src_node->id());
	}
      }
    }
  }

  // 冗長な through ノードを削除する．
  mMvnMgr->sweep();

  node_map = mNodeMap;

  return true;
}

// @brief module を生成する．
// @param[in] vl_module 対象のモジュール
MvnModule*
ReaderImpl::gen_module(const VlModule* vl_module)
{
  // ポート数，入出力のビット幅を調べる．
  ymuint np = vl_module->port_num();
  ymuint nio_all = vl_module->io_num();
  ymuint ni = 0;
  ymuint no = 0;
  ymuint nio = 0;
  for (ymuint i = 0; i < nio_all; ++ i) {
    const VlIODecl* io = vl_module->io(i);
    switch ( io->direction() ) {
    case kVlInput:  ++ ni; break;
    case kVlOutput: ++ no; break;
    case kVlInout:  ++ nio; break;
    default:
      MsgMgr::put_msg(__FILE__, __LINE__,
		      io->file_region(),
		      kMsgError,
		      "MVN_VL",
		      "Only Input/Output/Inout types are supported");
      return nullptr;
    }
  }
  vector<ymuint> ibw_array(ni);
  vector<ymuint> obw_array(no);
  vector<ymuint> iobw_array(nio);
  ni = 0;
  no = 0;
  nio = 0;
  for (ymuint i = 0; i < nio_all; ++ i) {
    const VlIODecl* io = vl_module->io(i);
    switch ( io->direction() ) {
    case kVlInput:  ibw_array[ni] = io->bit_size(); ++ ni; break;
    case kVlOutput: obw_array[no] = io->bit_size(); ++ no; break;
    case kVlInout:  iobw_array[nio] = io->bit_size(); ++ nio; break;
    default: break;
    }
  }

  MvnModule* module = mMvnMgr->new_module(vl_module->name(), np,
					  ibw_array, obw_array, iobw_array);

  // 入出力ノードの対応表を作る．
  ymuint i1 = 0;
  ymuint i2 = 0;
  ymuint i3 = 0;
  for (ymuint i = 0; i < nio_all; ++ i) {
    const VlIODecl* io = vl_module->io(i);
    const VlDecl* decl = io->decl();
    MvnNode* node = nullptr;
    switch ( io->direction() ) {
    case kVlInput:
      node = module->input(i1);
      ++ i1;
      break;

    case kVlOutput:
      node = module->output(i2);
      ++ i2;
      break;

    case kVlInout:
      node = module->inout(i3);
      ++ i3;
      break;

    default:
      ASSERT_NOT_REACHED;
      break;
    }
    mIODeclMap.add(decl, node);
  }

  // 宣言要素を生成する．
  bool stat = gen_decl(module, vl_module);
  if ( !stat ) {
    return nullptr;
  }

  // 要素を生成する．
  stat = gen_item(module, vl_module);
  if ( !stat ) {
    return nullptr;
  }

  // ポートの接続を行う．
  for (ymuint i = 0; i < np; ++ i) {
    const VlPort* port = vl_module->port(i);
    const VlExpr* expr = port->low_conn();
    if ( expr->is_operation() ) {
      ASSERT_COND( expr->op_type() == kVlConcatOp );
      ymuint n = expr->operand_num();
      vector<MvnPortRef> portref_list(n);
      for (ymuint j = 0; j < n; ++ j) {
	portref_list[j] = gen_portref(expr->operand(j));
      }
      mMvnMgr->init_port(module, i, portref_list, port->name());
    }
    else {
      MvnPortRef portref = gen_portref(expr);
      mMvnMgr->init_port(module, i, vector<MvnPortRef>(1, portref), port->name());
    }
  }

  return module;
}

// @brief 宣言要素を生成する．
// @param[in] module モジュール
// @param[in] vl_scope 対象のスコープ
// @retval true 成功した．
// @retval false エラーが起こった．
bool
ReaderImpl::gen_decl(MvnModule* module,
		     const VlNamedObj* vl_scope)
{
  // ネットの生成
  {
    vector<const VlDecl*> net_list;
    if ( mVlMgr.find_decl_list(vl_scope, vpiNet, net_list) ) {
      for (vector<const VlDecl*>::iterator p = net_list.begin();
	   p != net_list.end(); ++ p) {
	const VlDecl* vl_decl = *p;
	// 仮の through ノードに対応させる．
	ymuint bw = vl_decl->bit_size();
	MvnNode* node = mMvnMgr->new_through(module, bw);
	reg_node(vl_decl, node);

	// IO に接続している要素の場合，IOノード接続する．
	MvnNode* io_node = mIODeclMap.get(vl_decl);
	if ( io_node ) {
	  if ( io_node->type() == MvnNode::kInput ) {
	    reg_driver(node, Driver(vl_decl->file_region(), io_node));
	  }
	  else if ( io_node->type() == MvnNode::kOutput ) {
	    reg_driver(io_node, Driver(vl_decl->file_region(), node));
	  }
	}
      }
    }
  }

  // ネット配列の生成
  {
    vector<const VlDeclArray*> netarray_list;
    if ( mVlMgr.find_declarray_list(vl_scope, vpiNetArray, netarray_list) ) {
      for (vector<const VlDeclArray*>::iterator p = netarray_list.begin();
	   p != netarray_list.end(); ++ p) {
	const VlDeclArray* vl_decl = *p;
	ymuint array_size = vl_decl->array_size();
	for (ymuint i = 0; i < array_size; ++ i) {
	  // 仮の through ノードに対応させる．
	  ymuint bw = vl_decl->bit_size();
	  MvnNode* node = mMvnMgr->new_through(module, bw);
	  reg_node(vl_decl, i, node);
	}
      }
    }
  }

  // REG の生成
  {
    vector<const VlDecl*> reg_list;
    if ( mVlMgr.find_decl_list(vl_scope, vpiReg, reg_list) ) {
      for (vector<const VlDecl*>::iterator p = reg_list.begin();
	   p != reg_list.end(); ++ p) {
	const VlDecl* vl_decl = *p;

	// 仮の through ノードに対応させる．
	ymuint bw = vl_decl->bit_size();
	MvnNode* node = mMvnMgr->new_through(module, bw);
	reg_node(vl_decl, node);
	(void) mDeclHash.get_id(vl_decl);

	// IO に接続している要素の場合，IOノード接続する．
	MvnNode* io_node = mIODeclMap.get(vl_decl);
	if ( io_node ) {
	  if ( io_node->type() == MvnNode::kInput ) {
	    mMvnMgr->connect(io_node, 0, node, 0);
	  }
	  else if ( io_node->type() == MvnNode::kOutput ) {
	    mMvnMgr->connect(node, 0, io_node, 0);
	  }
	}
      }
    }
  }

  // REG配列の生成
  {
    vector<const VlDeclArray*> regarray_list;
    if ( mVlMgr.find_declarray_list(vl_scope, vpiRegArray, regarray_list) ) {
      for (vector<const VlDeclArray*>::iterator p = regarray_list.begin();
	   p != regarray_list.end(); ++ p) {
	const VlDeclArray* vl_decl = *p;
	ymuint array_size = vl_decl->array_size();
	(void) mDeclHash.get_id(vl_decl, 0);
	for (ymuint i = 0; i < array_size; ++ i) {
	  // 仮の through ノードに対応させる．
	  ymuint bw = vl_decl->bit_size();
	  MvnNode* node = mMvnMgr->new_through(module, bw);
	  reg_node(vl_decl, i, node);
	}
      }
    }
  }

  // 内部スコープ要素の生成
  {
    vector<const VlNamedObj*> scope_list;
    if ( mVlMgr.find_internalscope_list(vl_scope, scope_list) ) {
      for (vector<const VlNamedObj*>::iterator p = scope_list.begin();
	   p != scope_list.end(); ++ p) {
	const VlNamedObj* vl_scope1 = *p;
	bool stat = gen_decl(module, vl_scope1);
	if ( !stat ) {
	  return false;
	}
      }
    }
  }

  return true;
}

// @brief portref の実体化を行う．
// @param[in] expr 対象の式
// @return 生成されたポート参照式を返す．
MvnPortRef
ReaderImpl::gen_portref(const VlExpr* expr)
{
  const VlDecl* decl = expr->decl_obj();
  ASSERT_COND( decl != nullptr );
  MvnNode* node = mIODeclMap.get(decl);
  if ( node == nullptr ) {
    ostringstream buf;
    buf << decl->full_name() << ": Not found.";
    MsgMgr::put_msg(__FILE__, __LINE__,
		    decl->file_region(),
		    kMsgError,
		    "MVN_VL",
		    buf.str());
    return MvnPortRef();
  }

  if ( expr->is_bitselect() ) {
    ASSERT_COND( node != nullptr );
    ASSERT_COND( expr->is_constant_select() );
    ASSERT_COND( expr->declarray_dimension() == 0 );
    ymuint bitpos = expr->index_val();
    return MvnPortRef(node, bitpos);
  }

  if ( expr->is_partselect() ) {
    ASSERT_COND( node != nullptr );
    ASSERT_COND( expr->is_constant_select() );
    ASSERT_COND( expr->declarray_dimension() == 0 );
    ymuint msb = expr->left_range_val();
    ymuint lsb = expr->right_range_val();
    return MvnPortRef(node, msb, lsb);
  }

  return MvnPortRef(node);
}

// @brief 左辺式に接続する．
// @param[in] dst_node 左辺に対応するノード
// @param[in] expr 左辺式
// @param[in] src_node 右辺に対応するノード
void
ReaderImpl::connect_lhs(MvnNode* dst_node,
			const VlExpr* expr,
			MvnNode* src_node,
			const FileRegion& src_loc)
{
  if ( expr->is_primary() ) {
    reg_driver(dst_node, Driver(src_loc, src_node));
  }
  else if ( expr->is_bitselect() ) {
    ASSERT_COND( expr->is_constant_select() );
    const VlDeclBase* decl = expr->decl_base();
    ASSERT_COND( decl != nullptr );
    ymuint offset;
    if ( !decl->calc_bit_offset(expr->index_val(), offset) ) {
      MsgMgr::put_msg(__FILE__, __LINE__,
		      expr->file_region(),
		      kMsgError,
		      "MVN_VL",
		      "Index is out of range.");
      return;
    }
    reg_driver(dst_node, Driver(src_loc, src_node, offset));
  }
  else if ( expr->is_partselect() ) {
    ASSERT_COND( expr->is_constant_select() );
    const VlDeclBase* decl = expr->decl_base();
    ASSERT_COND( decl != nullptr );
    ymuint msb;
    if ( !decl->calc_bit_offset(expr->left_range_val(), msb) ) {
      MsgMgr::put_msg(__FILE__, __LINE__,
		      expr->file_region(),
		      kMsgError,
		      "MVN_VL",
		      "Left index is out of range.");
      return;
    }
    ymuint lsb;
    if ( !decl->calc_bit_offset(expr->right_range_val(), lsb) ) {
      MsgMgr::put_msg(__FILE__, __LINE__,
		      expr->file_region(),
		      kMsgError,
		      "MVN_VL",
		      "Right index is out of range.");
      return;
    }
    ASSERT_COND( src_node->bit_width() == msb - lsb + 1 );
    reg_driver(dst_node, Driver(src_loc, src_node, msb, lsb));
  }
}

// @brief 宣言要素に対応するノードを登録する．
// @param[in] decl 宣言要素
// @param[in] node 登録するノード
void
ReaderImpl::reg_node(const VlDecl* decl,
		     MvnNode* node)
{
  mGlobalEnv.add(decl, node);
  mNodeMap.reg_node(node->id(), decl);
}

// @brief 入出力宣言要素に対応するノードを登録する．
// @param[in] decl 宣言要素
// @param[in] node 登録するノード
void
ReaderImpl::reg_ionode(const VlDecl* decl,
		       MvnNode* node)
{
  mIODeclMap.add(decl, node);
  mGlobalEnv.add(decl, node);
  mNodeMap.reg_node(node->id(), decl);
}

// @brief 宣言要素に対応するノードを登録する．
// @param[in] decl 宣言要素(配列型)
// @param[in] offset オフセット
// @param[in] node 登録するノード
void
ReaderImpl::reg_node(const VlDeclArray* decl,
		     ymuint offset,
		     MvnNode* node)
{
  mGlobalEnv.add(decl, offset, node);
  mNodeMap.reg_node(node->id(), decl, offset);
}

// @brief ドライバーを登録する．
// @param[in] node 左辺に対応するノード
// @param[in] driver ドライバー
void
ReaderImpl::reg_driver(MvnNode* node,
		       const Driver& driver)
{
  if ( debug_driver ) {
    cerr << "reg_driver(" << node->id()
	 << ", " << driver.rhs_node()->id();
    if ( driver.has_bitselect() ) {
      cerr << "[" << driver.index() << "]";
    }
    else if ( driver.has_partselect() ) {
      cerr << "[" << driver.msb() << ":" << driver.lsb() << "]";
    }
    cerr << ")"
	 << driver.loc()
	 << endl;
  }
  driver_list(node).push_back(driver);
}

// @brief ドライバーリストを取り出す．
// @param[in] node 対応するノード
// @note なければ空のリストを作る．
vector<Driver>&
ReaderImpl::driver_list(MvnNode* node)
{
  ASSERT_COND( node != nullptr );
  ymuint id = node->id();
  while ( mDriverList.size() <= id ) {
    mDriverList.push_back(vector<Driver>());
  }
  return mDriverList[id];
}

// @brief 複数のドライバがある時にエラーメッセージを出力する．
void
ReaderImpl::error_drivers(MvnNode* node,
			  const Driver& driver1,
			  const Driver& driver2)
{
  ostringstream buf;
  buf << "More than one drivers. "
      << "Previous driver is " << driver1.loc();
  MsgMgr::put_msg(__FILE__, __LINE__,
		  driver2.loc(),
		  kMsgError,
		  "MVN_VL",
		  buf.str());
}

END_NAMESPACE_YM_NETWORKS_MVN_VERILOG
