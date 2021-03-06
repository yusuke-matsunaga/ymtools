﻿
/// @file Parser.cc
/// @brief Parser の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "Parser.h"

#include "Lex.h"
#include "LexModuleState.h"
#include "PtMgr.h"
#include "PtiFactory.h"
#include "PtiDecl.h"
#include "YmVerilog/pt/PtUdp.h"
#include "YmVerilog/pt/PtExpr.h"

#include "YmUtils/MsgMgr.h"


BEGIN_NAMESPACE_YM_VERILOG

//////////////////////////////////////////////////////////////////////
// UDP 関係
//////////////////////////////////////////////////////////////////////

// @brief UDP定義の開始
// - port list の初期化
// - iohead list の初期化
// - declhead list の初期化
// - UDP entry list の初期化
// を行う．
void
Parser::init_udp()
{
  mPortList.clear();
  mCurIOHeadList = &mModuleIOHeadList;
  mCurDeclHeadList = &mModuleDeclHeadList;

  mCurIOHeadList->clear();
  mIOItemList.clear();

  mCurDeclHeadList->clear();
  mDeclItemList.clear();

  mUdpEntryList.clear();
}

// @brief UDP 定義の終了
void
Parser::end_udp()
{
}

// UDP を生成する．(Verilog-1995)
void
Parser::new_Udp1995(const FileRegion& file_region,
		    const char* udp_name,
		    const char* init_name,
		    const FileRegion& init_loc,
		    const PtExpr* init_value,
		    PtrList<const PtAttrInst>* ai_list)
{
  PtIOHeadArray iohead_array = get_module_io_array();
  PtDeclHeadArray decl_array = get_module_decl_array();

  const PtIOItem* out_item = nullptr;
  bool is_seq = false;

  bool sane = true;

  // Verilog1995 の形式の時には YACC の文法では規定できないので，
  // 1. port_list に現れる名前が portdecl_list 中に存在すること．
  // 2. その最初の名前は output であること．
  // 3. それ以外はすべて input であること
  // の確認を行う．
  // まず portdecl_list の各要素を名前をキーにした連想配列に格納する．
  // ついでに output の数を数える．
  HashMap<string, const PtIOItem*> iomap;
  for (ymuint i = 0; i < iohead_array.size(); ++ i) {
    const PtIOHead* io = iohead_array[i];
    if ( io->type() == kPtIO_Output ) {
      if ( out_item ) {
	// 複数の出力宣言があった．
	MsgMgr::put_msg(__FILE__, __LINE__,
			io->file_region(),
			kMsgError,
			"PARS",
			"More than two output declarations");
	sane = false;
	break;
      }

      // これは YACC の文法が正しくかけていれば成り立つはず．
      ASSERT_COND(io->item_num() == 1 );

      out_item = io->item(0);

      if ( io->aux_type() == kVpiAuxReg ) {
	is_seq = true;
      }
    }
    for (ymuint j = 0; j < io->item_num(); ++ j) {
      const PtIOItem* elem = io->item(j);
      if ( iomap.check(elem->name()) ) {
	// 二重登録
	ostringstream buf;
	buf << elem->name() << ": Defined more than once.";
	MsgMgr::put_msg(__FILE__, __LINE__,
			elem->file_region(),
			kMsgError,
			"PARS",
			buf.str());
	sane = false;
	break;
      }
      iomap.add(elem->name(), elem);
    }
  }

  // port_list に現れる名前が iolist 中にあるか調べる．
  PtiPortArray port_array = get_port_array();
  for (ymuint i = 0; i < port_array.size(); ++ i) {
    const PtiPort* port = port_array[i];
    const char* port_name = port->ext_name();
    const PtIOItem* ioelem;
    if ( !iomap.find(port_name, ioelem) ) {
      ostringstream buf;
      buf << "\"" << port_name << "\" undefined.";
      MsgMgr::put_msg(__FILE__, __LINE__,
		      file_region,
		      kMsgError,
		      "PARS",
		      buf.str());
      sane = false;
      break;
    }
    if ( i == 0 ) {
      if ( out_item != ioelem ) {
	// 最初の名前は output でなければならない．
	ostringstream buf;
	buf << port_name << " must be an output.";
	MsgMgr::put_msg(__FILE__, __LINE__,
			ioelem->file_region(),
			kMsgError,
			"PARS",
			buf.str());
	sane = false;
	break;
      }
    }
    iomap.erase(port_name);
  }

  if ( iomap.num() > 0 ) {
    // iolist 中のみに現れる要素がある．
    for (HashMapIterator<string, const PtIOItem*> q = iomap.begin();
	 q != iomap.end(); ++ q) {
      const PtIOItem* ioelem = q.value();
      ostringstream buf;
      buf << "\"" << ioelem->name() << "\" does not appear in portlist.";
      MsgMgr::put_msg(__FILE__, __LINE__,
		      ioelem->file_region(),
		      kMsgError,
		      "PARS",
		      buf.str());
    }
    sane = false;
  }

  // 次に decl_list の要素数が1以下であり，
  // さらにその要素が REG で名前が出力名と一致することを確認する．
  // ちなみに YACC の文法から REG 以外の宣言要素はありえない．
  if ( decl_array.size() > 1 ) {
    // 二つ以上の reg 宣言があった．
    MsgMgr::put_msg(__FILE__, __LINE__,
		    decl_array[1]->file_region(),
		    kMsgError,
		    "PARS",
		    "More than two 'reg' declarations.");
    sane = false;
  }
  else if ( decl_array.size() == 1 ) {
    const PtDeclHead* reghead = decl_array[0];
    if ( reghead ) {
      is_seq = true;
      ASSERT_COND(reghead->type() == kPtDecl_Reg );
      ASSERT_COND(reghead->item_num() == 1 );
      const PtDeclItem* regitem = reghead->item(0);
      ASSERT_COND(regitem );
      if ( strcmp(regitem->name(), out_item->name()) != 0 ) {
	// output と名前が違う
	ostringstream buf;
	buf << "Reg name \"" << regitem->name()
	    << "\" differes from output name \""
	    << out_item->name() << "\".";
	MsgMgr::put_msg(__FILE__, __LINE__,
			regitem->file_region(),
			kMsgError,
			"PARS",
			buf.str());
	sane = false;
      }
    }
  }

  if ( sane ) {
    new_Udp(file_region,
	    udp_name,
	    init_name,
	    init_loc,
	    init_value,
	    ai_list,
	    is_seq,
	    out_item,
	    port_array,
	    iohead_array);
  }
  end_udp();
}

// UDP を生成する．(Verilog-2001)
void
Parser::new_Udp2001(const FileRegion& file_region,
		    const char* udp_name,
		    const char* init_name,
		    const FileRegion& init_loc,
		    const PtExpr* init_value,
		    PtrList<const PtAttrInst>* ai_list)
{
  PtIOHeadArray iohead_array = get_module_io_array();

  bool is_seq = false;

  // YACC の文法が正しく書かれていれば最初のヘッダが出力で
  // 要素数が1となっているはず．
  ASSERT_COND(iohead_array.size() > 0 );
  const PtIOHead* out_head = iohead_array[0];
  ASSERT_COND(out_head->type() == kPtIO_Output );
  ASSERT_COND(out_head->item_num() == 1 );
  const PtIOItem* out_item = out_head->item(0);

  if ( out_head->aux_type() == kVpiAuxReg ) {
    is_seq = true;
  }
  // 残りの要素は入力になっているはず．

  // iohead_array から port_array を生成する．
  PtiPortArray port_array = new_PortArray(iohead_array);

  new_Udp(file_region,
	  udp_name,
	  init_name,
	  init_loc,
	  init_value,
	  ai_list,
	  is_seq,
	  out_item,
	  port_array,
	  iohead_array);
  end_udp();
}

// @brief new_Udp の下請け関数
void
Parser::new_Udp(const FileRegion& file_region,
		const char* udp_name,
		const char* init_name,
		const FileRegion& init_loc,
		const PtExpr* init_value,
		PtrList<const PtAttrInst>* ai_list,
		bool is_seq,
		const PtIOItem* out_item,
		PtiPortArray port_array,
		PtIOHeadArray iohead_array)
{
  const PtUdp* udp = nullptr;
  if ( is_seq ) {
    // 初期値の設定がある．
    if ( init_name ) {

      // init_name が正しいかチェックする．
      if ( strcmp(init_name, out_item->name()) != 0 ) {
	// output 文の名前と異なる．
	ostringstream buf;
	buf << "Lhs of initial \"" << init_name
	    << "\" differes from output name \""
	    << out_item->name() << "\".";
	MsgMgr::put_msg(__FILE__, __LINE__,
			init_loc,
			kMsgError,
			"PARS",
			buf.str());
	return;
      }

      if ( out_item->init_value() ) {
	// output 文にも初期値割り当てがある．
	// これは warning にする．
	MsgMgr::put_msg(__FILE__, __LINE__,
			init_value->file_region(),
			kMsgWarning,
			"PARS",
			"Both output declaration and initial block"
			" have the initial values,"
			" output declarations's initial value is ignored.");
      }
    }

    // このあと elaboration で注意が必要なのは init_value.
    // 場合によってはこれが nullptrで outhead->top()->init_value()
    // が空でない場合がある．
    udp = mFactory.new_SeqUdp(file_region,
			      udp_name,
			      port_array,
			      iohead_array,
			      init_value,
			      get_udp_entry_array());
  }
  else {
    if ( init_name ) {
      // sequential primitive でなければ初期値を持てない．
      MsgMgr::put_msg(__FILE__, __LINE__,
		      init_loc,
		      kMsgError,
		      "PARS",
		      "Combinational primitive can not have the initial value.");
      return;
    }
    udp = mFactory.new_CmbUdp(file_region,
			      udp_name,
			      port_array,
			      iohead_array,
			      get_udp_entry_array());
  }

  mPtMgr.reg_udp(udp);
  reg_attrinst(udp, ai_list);
}

// @brief combinational UDP 用のテーブルエントリの生成
// @param[in] fr ファイル位置の情報
// @param[in] output_loc 出力記号の位置
// @param[in] output_symbol 出力記号
void
Parser::new_UdpEntry(const FileRegion& fr,
		     const FileRegion& output_loc,
		     char output_symbol)
{
  const PtUdpValue* output = mFactory.new_UdpValue(output_loc, output_symbol);
  add_udp_entry( mFactory.new_UdpEntry(fr, get_udp_value_array(), output) );
}

// @brief sequential UDP 用のテーブルエントリの生成
// @param[in] fr ファイル位置の情報
// @param[in] current_loc 現状態記号の位置
// @param[in] current_symbol 現状態記号
// @param[in] output_loc 出力記号の位置
// @param[in] output_symbol 出力記号
void
Parser::new_UdpEntry(const FileRegion& fr,
		     const FileRegion& current_loc,
		     char current_symbol,
		     const FileRegion& output_loc,
		     char output_symbol)
{
  const PtUdpValue* current = mFactory.new_UdpValue(current_loc,
						    current_symbol);
  const PtUdpValue* output = mFactory.new_UdpValue(output_loc,
						   output_symbol);
  add_udp_entry( mFactory.new_UdpEntry(fr, get_udp_value_array(),
				       current, output) );
}

// @brief UdpEntry を追加する．
inline
void
Parser::add_udp_entry(const PtUdpEntry* entry)
{
  mUdpEntryList.push_back(entry);
}

// @brief UdpEntry のリストを配列に変換する．
inline
PtUdpEntryArray
Parser::get_udp_entry_array()
{
  return mUdpEntryList.to_array(mAlloc);
}

// @brief UDP のテーブルエントリの要素の値の生成
// @param[in] fr ファイル位置の情報
// @param[in] symbol シンボル
// @return 生成された値
void
Parser::new_UdpValue(const FileRegion& fr,
		     char symbol)
{
  add_udp_value( mFactory.new_UdpValue(fr, symbol) );
}

// @brief UDP のテーブルエントリの要素の値の生成
// @param[in] fr ファイル位置の情報
// @param[in] symbol1, symbol2 シンボル
// @return 生成された値
void
Parser::new_UdpValue(const FileRegion& fr,
		     char symbol1,
		     char symbol2)
{
  add_udp_value( mFactory.new_UdpValue(fr, symbol1, symbol2) );
}

// @brief UdpValue のリストを初期化する．
void
Parser::init_udp_value_list()
{
  mUdpValueList.clear();
}

// @brief UdpValue を追加する．
inline
void
Parser::add_udp_value(const PtUdpValue* value)
{
  mUdpValueList.push_back(value);
}

// @brief UdpValue のリストを配列に変換する．
inline
PtUdpValueArray
Parser::get_udp_value_array()
{
  return mUdpValueList.to_array(mAlloc);
}

// @brief ポートリストを配列に変換する．
inline
PtiPortArray
Parser::get_port_array()
{
  return mPortList.to_array(mAlloc);
}

// @brief IO宣言リストを配列に変換する．
inline
PtIOHeadArray
Parser::get_module_io_array()
{
  return mModuleIOHeadList.to_array(mAlloc);
}

// @brief module 用の宣言リストを配列に変換する．
inline
PtDeclHeadArray
Parser::get_module_decl_array()
{
  return mModuleDeclHeadList.to_array(mAlloc);
}

END_NAMESPACE_YM_VERILOG
