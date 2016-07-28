
/// @file libym_mislib/MislibParserImpl.cc
/// @brief MislibParserImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "MislibParserImpl.h"
#include "MislibMgrImpl.h"
#include "ym_utils/MsgMgr.h"


BEGIN_NONAMESPACE

// MislibLex::read_token() をデバッグする時に true にする．
bool debug_read_token = false;

END_NONAMESPACE

BEGIN_NAMESPACE_YM_MISLIB

#include "mislib_grammer.hh"

//////////////////////////////////////////////////////////////////////
// クラス MislibParserImpl
//////////////////////////////////////////////////////////////////////

// コンストラクタ
MislibParserImpl::MislibParserImpl()
{
}

// デストラクタ
MislibParserImpl::~MislibParserImpl()
{
}


BEGIN_NONAMESPACE

// 論理式中に現れる名前を ipin_set に積む．
void
get_ipin_names(const MislibNode* expr_node,
	       HashSet<ShString>& ipin_set)
{
  ShString name;

  switch ( expr_node->type() ) {
  case MislibNode::kConst0:
  case MislibNode::kConst1:
    // 定数は無視
    return;

  case MislibNode::kStr:
    ipin_set.add(expr_node->str());
    break;

  case MislibNode::kNot:
    get_ipin_names(expr_node->child1(), ipin_set);
    break;

  case MislibNode::kAnd:
  case MislibNode::kOr:
  case MislibNode::kXor:
    get_ipin_names(expr_node->child1(), ipin_set);
    get_ipin_names(expr_node->child2(), ipin_set);
    break;

  default:
    assert_not_reached(__FILE__, __LINE__);
  }
}

END_NONAMESPACE

// @brief mislib ファイルを読み込んでライブラリを生成する．
// @param[in] filename ファイル名
// @param[in] mgr MislibNode を管理するオブジェクト
// @return パース木の根のノードを返す．
// @note 読み込みが失敗したら false を返す．
bool
MislibParserImpl::read_file(const string& filename,
			    MislibMgrImpl* mgr)
{
  int yyparse(MislibParserImpl& parser);

  if ( !mLex.open_file(filename) ) {
    // エラー
    ostringstream buf;
    buf << filename << " : No such file.";
    MsgMgr::put_msg(__FILE__, __LINE__,
		    FileRegion(),
		    kMsgFailure,
		    "MISLIB_PARSER",
		    buf.str());
    return false;
  }

  // 初期化
  mMislibMgr = mgr;
  mMislibMgr->clear();

  ymuint prev_errnum = MsgMgr::error_num();

  // パース木を作る．
  // 結果は MislibMgrImpl が持っている．
  yyparse(*this);

  if ( MsgMgr::error_num() > prev_errnum ) {
    // 異常終了
    return false;
  }

  // 重複したセル名がないかチェック
  // また，セル内のピン名が重複していないか，出力ピンの論理式に現れるピン名
  // と入力ピンに齟齬がないかもチェックする．
  const MislibNode* gate_list = mgr->gate_list();
  HashMap<ShString, const MislibNode*> cell_map;
  for (const MislibNode* gate = gate_list->top(); gate; gate = gate->next()) {
    assert_cond( gate->type() == MislibNode::kGate, __FILE__, __LINE__);
    ShString name = gate->name()->str();
    const MislibNode* node;
    if ( cell_map.find(name, node) ) {
      ostringstream buf;
      buf << "Cell name, " << name << " is defined more than once. "
	  << "Previous definition is " << node->name()->loc() << ".";
      MsgMgr::put_msg(__FILE__, __LINE__,
		      gate->name()->loc(),
		      kMsgError,
		      "MISLIB_PARSER",
		      buf.str());
      // このセルについては以降のチェックをスキップする．
      continue;
    }
    // 情報を登録する．
    cell_map.add(name, gate);

    // 入力ピン名のチェック
    const MislibNode* ipin_list = gate->ipin_list();
    if ( ipin_list->type() == MislibNode::kList ) {
      // 通常の入力ピン定義の場合
      HashMap<ShString, const MislibNode*> ipin_map;
      for (const MislibNode* ipin = ipin_list->top(); ipin; ipin = ipin->next()) {
	assert_cond( ipin->type() == MislibNode::kPin, __FILE__, __LINE__);
	ShString name = ipin->name()->str();
	const MislibNode* node;
	if ( ipin_map.find(name, node) ) {
	  ostringstream buf;
	  buf << "Pin name, " << name << " is defined more than once. "
	      << "Previous definition is "
	      << node->name()->loc() << ".";
	  MsgMgr::put_msg(__FILE__, __LINE__,
			  ipin->name()->loc(),
			  kMsgError,
			  "MISLIB_PARSER",
			  buf.str());
	}
	else {
	  ipin_map.add(name, ipin);
	}
      }
      // 論理式に現れる名前の集合を求める．
      HashSet<ShString> ipin_set;
      get_ipin_names(gate->opin_expr(), ipin_set);
      for (HashMapIterator<ShString, const MislibNode*> p = ipin_map.begin();
	   p != ipin_map.end(); ++ p) {
	ShString name = p.key();
	if ( !ipin_set.check(name) ) {
	  // ピン定義に現れる名前が論理式中に現れない．
	  // エラーではないが，このピンのタイミング情報は意味をもたない．
	  ostringstream buf;
	  buf << "Input pin, " << name
	      << " does not appear in the logic expression. "
	      << "Timing information will be ignored.";
	  MsgMgr::put_msg(__FILE__, __LINE__,
			  p.value()->loc(),
			  kMsgWarning,
			  "MISLIB_PARSER",
			  buf.str());
	}
      }
      for (HashSetIterator<ShString> p = ipin_set.begin();
	   p != ipin_set.end(); ++ p) {
	ShString name = p.key();
	if ( !ipin_map.check(name) ) {
	  // 論理式中に現れる名前の入力ピンが存在しない．
	  // これはエラー
	  ostringstream buf;
	  buf << name << " appears in the logic expression, "
	      << "but is not defined in PIN statement.";
	  MsgMgr::put_msg(__FILE__, __LINE__,
			  gate->opin_expr()->loc(),
			  kMsgError,
			  "MISLIB_PARSER",
			  buf.str());
	}
      }
    }
    else if ( ipin_list->type() == MislibNode::kPin ) {
      // ワイルドカードの場合
      // シンタックス的にはエラーとなることはない．
    }
    else {
      // yacc の文法からありえない．
      assert_not_reached(__FILE__, __LINE__);
    }
  }

  if ( MsgMgr::error_num() > prev_errnum ) {
    // 異常終了
    return false;
  }
  // 読み込みまではうまくいった．
  return true;
}

// @brief 字句解析を行う．
// @param[out] lval トークンの値を格納する変数
// @param[out] lloc トークンの位置を格納する変数
// @return トークンの型を返す．
int
MislibParserImpl::scan(MislibNodeImpl*& lval,
		       FileRegion& lloc)
{
  int tok = mLex.read_token(lloc);

  switch ( tok ) {
  case STR:
    lval = mMislibMgr->new_str(lloc, ShString(mLex.cur_string()));
    break;

  case NUM:
    lval = mMislibMgr->new_num(lloc, mLex.cur_num());
    break;

  case NONINV:
    lval = mMislibMgr->new_noninv(lloc);
    break;

  case INV:
    lval = mMislibMgr->new_inv(lloc);
    break;

  case UNKNOWN:
    lval = mMislibMgr->new_unknown(lloc);
    break;

  case CONST0:
    lval = mMislibMgr->new_const0(lloc);
    break;

  case CONST1:
    lval = mMislibMgr->new_const1(lloc);
    break;

  default:
    break;
  }

  if ( debug_read_token ) {
    cout << "MislibParserImpl::scan(): ";
    switch ( tok ) {
    case STR:
      cout << "STR(" << mLex.cur_string() << ")" << endl;
      break;

    case NUM:
      cout << "NUM(" << mLex.cur_num() << ")" << endl;
      break;

    case NONINV:
      cout << "NONINV" << endl;
      break;

    case INV:
      cout << "INV" << endl;
      break;

    case UNKNOWN:
      cout << "UNKNOWN" << endl;
      break;

    case CONST0:
      cout << "CONST0" << endl;
      break;

    case CONST1:
      cout << "CONST1" << endl;
      break;

    case LP:
      cout << "LP" << endl;
      break;

    case RP:
      cout << "RP" << endl;
      break;

    case SEMI:
      cout << "SEMI" << endl;
      break;

    case EQ:
      cout << "EQ" << endl;
      break;

    case GATE:
      cout << "GATE" << endl;
      break;

    case PIN:
      cout << "PIN" << endl;
      break;

    case ERROR:
      cout << "ERROR" << endl;
      break;

    case PLUS:
      cout << "PLUS" << endl;
      break;

    case HAT:
      cout << "HAT" << endl;
      break;

    case STAR:
      cout << "STAR" << endl;
      break;

    case NOT:
      cout << "NOT" << endl;
      break;

    case EOF:
      cout << "EOF" << endl;
      break;

    default:
      cout << tok << endl;
      break;
    }
  }

  return tok;
}

// @brief エラーメッセージを出力する．
void
MislibParserImpl::error(const FileRegion& loc,
			const char* msg)
{
  string buff;
  const char* msg2;
  // 好みの問題だけど "parse error" よりは "syntax error" の方が好き．
  if ( !strncmp(msg, "parse error", 11) ) {
    buff ="syntax error";
    buff += (msg + 11);
    msg2 = buff.c_str();
  }
  else {
    msg2 = msg;
  }

  MsgMgr::put_msg(__FILE__, __LINE__,
		  loc,
		  kMsgError,
		  "MISLIB_PARSER",
		  msg2);
}

END_NAMESPACE_YM_MISLIB
