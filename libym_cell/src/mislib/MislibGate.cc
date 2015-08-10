﻿
/// @file MislibGate.cc
/// @brief MislibGate の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "MislibGate.h"


BEGIN_NAMESPACE_YM_MISLIB

//////////////////////////////////////////////////////////////////////
// クラス MislibGate
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] loc 位置情報
// @param[in] name 名前を表すパース木
// @param[in] area 面積を表すパース木
// @param[in] opin_name 出力ピン名を表すパース木
// @param[in] opin_expr 出力の論理式を表すパース木
// @param[in] ipin_top 先頭の入力ピンを表すパース木
MislibGate::MislibGate(const FileRegion& loc,
		       const MislibNode* name,
		       const MislibNode* area,
		       const MislibNode* opin_name,
		       const MislibNode* opin_expr,
		       const MislibNode* ipin_top) :
  MislibNodeImpl(loc),
  mName(name),
  mArea(area),
  mOpinName(opin_name),
  mOpinExpr(opin_expr),
  mIpinTop(ipin_top),
  mNext(nullptr)
{
}

// @brief デストラクタ
MislibGate::~MislibGate()
{
}

// @brief 種類を取り出す．
MislibNode::tType
MislibGate::type() const
{
  return kGate;
}

// @brief ピン名/ゲート名を表すオブジェクトを取り出す．
const MislibNode*
MislibGate::name() const
{
  return mName;
}

// @brief 面積を表すオブジェクトを返す．
const MislibNode*
MislibGate::area() const
{
  return mArea;
}

// @brief 出力ピン名を表すオブジェクトを返す．
const MislibNode*
MislibGate::opin_name() const
{
  return mOpinName;
}

// @brief 出力の論理式を表すオブジェクトを返す．
const MislibNode*
MislibGate::opin_expr() const
{
  return mOpinExpr;
}

// @brief 先頭の入力ピンのリストを表すオブジェクトを返す．
const MislibNode*
MislibGate::ipin_top() const
{
  return mIpinTop;
}

// 次の要素を設定する．
void
MislibGate::set_next(MislibNodeImpl* next)
{
  mNext = next;
}

// 次の要素を取り出す．
const MislibNode*
MislibGate::next() const
{
  return mNext;
}

// @brief 内容を出力する．
void
MislibGate::dump(ostream& s) const
{
  s << "<GATE>" << endl;
  dump_loc(s);

  s << "<NAME>" << endl;
  name()->dump(s);
  s << "</NAME>" << endl;

  s << "<AREA>" << endl;
  area()->dump(s);
  s << "</AREA>" << endl;

  s << "<OPIN_NAME>" << endl;
  opin_name()->dump(s);
  s << "</OPIN_NAME>" << endl;

  s << "<OPIN_EXPR>" << endl;
  opin_expr()->dump(s);
  s << "</OPIN_EXPR>" << endl;

  s << "<IPIN_LIST>" << endl;
  for (const MislibNode* pin = ipin_top(); pin != nullptr; pin = pin->next()) {
    pin->dump(s);
  }
  s << "</IPIN_LIST>" << endl;

  s << "</GATE>" << endl;
}

END_NAMESPACE_YM_MISLIB
