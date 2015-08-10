﻿
/// @file BNetwork.cc
/// @brief BNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmNetworks/BNetwork.h"
#include "YmNetworks/BNetManip.h"
#include "StrBNodeMap.h"
#include "BNodeMgr.h"


BEGIN_NAMESPACE_YM_NETWORKS_BNET

// コンストラクタ
BNetwork::BNetwork() :
  mModelName("network"),
  mPiList(1),
  mPoList(1),
  mLogicList(1),
  mLatchList(1),
  mNodeList(0),
  mNameMap(new StrBNodeMap),
  mPoMap(new StrBNodeMap),
  mNameMgr("[", "]")
{
}

// コピーコンストラクタ
BNetwork::BNetwork(const BNetwork& src) :
  mPiList(1),
  mPoList(1),
  mLogicList(1),
  mLatchList(1),
  mNodeList(0),
  mNameMap(new StrBNodeMap),
  mPoMap(new StrBNodeMap),
  mNameMgr(src.mNameMgr.prefix().c_str(), src.mNameMgr.suffix().c_str())
{
  mItvlMgr.erase(0);

  copy(src);
}

// デストラクタ
BNetwork::~BNetwork()
{
  clear();

  BNodeMgr& mgr = BNodeMgr::the_obj();
  ymuint n = mNodeVector.size();
  for (ymuint i = 0; i < n; ++ i) {
    mgr.free_node(mNodeVector[i]);
  }

  delete mNameMap;
  delete mPoMap;
}

// 代入演算子
const BNetwork&
BNetwork::operator=(const BNetwork& src)
{
  // 同じものどうしでないか確認する．
  if ( this != &src ) {
    clear();
    copy(src);
  }
  return *this;
}

// nameという名前をもつノードを取り出す．
// 該当する節点が無い場合には nullptr を返す．
BNode*
BNetwork::find_node(const char* name) const
{
  BNode::tId id = mNameMap->find_id(name);
  if ( id == 0 ) {
    return nullptr;
  }
  return mNodeVector[id];
}

// nameという名前をもつ外部出力節点を取り出す．
// 該当する節点が無い場合には nullptr を返す．
BNode*
BNetwork::find_ponode(const char* name) const
{
  BNode::tId id = mPoMap->find_id(name);
  if (id == 0 ) {
    return nullptr;
  }
  return mNodeVector[id];
}

// リテラル数を得る．
// ただ単にすべての中間節点からLitNum()の結果を聞いて回って，
// それを足しているだけ．
ymuint
BNetwork::litnum() const
{
  ymuint ans = 0;
  for (BNodeList::const_iterator p = logic_nodes_begin();
       p != logic_nodes_end(); ++p) {
    BNode* node = *p;
    ans += node->litnum();
  }
  return ans;
}

// SOP形式のリテラル数を得る．
// ただ単にすべての中間節点からSopLitNum()の結果を聞いて回って，
// それを足しているだけ．
ymuint
BNetwork::sop_litnum() const
{
  ymuint ans = 0;
  for (BNodeList::const_iterator p = logic_nodes_begin();
       p != logic_nodes_end(); ++p) {
    BNode* node = *p;
    ans += node->sop_litnum();
  }
  return ans;
}

// 構造が変わったことを記録しておく関数
void
BNetwork::structure_changed()
{
  // 今のところやることはない．
}

// 変数名生成規則に関するメンバ関数
// 自動に生成する名前の prefix と suffix を変更する．
// 生成名は sprintf("%s%d%s", prefix, id, suffix)
// となる．
void
BNetwork::change_name_rule(const char* prefix,
			   const char* suffix)
{
  mNameMgr.change(prefix, suffix);

  // 現在存在している名前を新しい mNameMgr に登録する．
  for (BNodeList::const_iterator p = mNodeList.begin();
       p != mNodeList.end(); ++ p) {
    BNode* node = *p;
    const char* name = node_name(node);
    if ( !node->is_output() && name != nullptr ) {
      mNameMgr.add(name);
    }
  }
}

// 自動生成名規則の prefix と suffix を得る．
void
BNetwork::name_rule(string& prefix,
		    string& suffix) const
{
  prefix = mNameMgr.prefix();
  suffix = mNameMgr.suffix();
}

// @brief モデル名を設定する．
// @param[in] name 設定するモデル名
void
BNetwork::set_model_name(const char* name)
{
  mModelName = name;
}

// @brief モデル名を設定する．
// @param[in] name 設定するモデル名
void
BNetwork::set_model_name(const string& name)
{
  mModelName = name;
}

// 以前の内容は捨てて空のネットワークにする．
void
BNetwork::clear()
{
  // 節点は互いに接続しあっているので下手にオブジェクトを
  // 削除するといろいろと不都合が起きる．

  // 接続を切る．
  BNodeVector null_fanins(0);
  for (BNodeList::const_iterator p = mNodeList.begin();
       p != mNodeList.end(); ++ p) {
    BNode* node = *p;
    set_node_fanins(node, null_fanins);
  }

  mPiList.clear();
  mPoList.clear();
  mLogicList.clear();
  mLatchList.clear();
  mNodeList.clear();

  mNameMap->clear();
  mPoMap->clear();

  mItvlMgr.clear();
  mItvlMgr.erase(0);
  mNameMgr.clear();

  // 変化した事を記録しておく．
  structure_changed();
  mTraceMgr.prop_event(BNetChg::all_clear());

  // mNodeVector はなにもしない．
}

// コピーを行なう関数
void
BNetwork::copy(const BNetwork& src)
{
  // ネットワークは『真っ白』でなければならない．
  ASSERT_COND(input_num() == 0 );
  ASSERT_COND(output_num() == 0 );
  ASSERT_COND(logic_node_num() == 0 );
  ASSERT_COND(latch_node_num() == 0 );

  // 対応付けのための連想配列
  size_t n = src.max_node_id();
  vector<BNode*> node_assoc(n, nullptr);

  set_model_name(src.model_name());

  BNetManip manip(this);

  //////////////////////////////////////////////////////////////////////
  // PHASE-1: ノードを作って node_assoc に入れる．
  //          内部情報はまだ設定できない．
  //          この時点では型と名前だけ．
  //////////////////////////////////////////////////////////////////////

  // 外部入力ノード
  for (BNodeList::const_iterator p = src.inputs_begin();
       p != src.inputs_end(); ++ p) {
    BNode* src_node = *p;
    const char* src_name = src.node_name(src_node);
    BNode* dst_node = manip.new_input(src_name);
    ASSERT_COND( dst_node != 0 );
    node_assoc[src_node->id()] = dst_node;
  }

  // 外部出力ノード
  for (BNodeList::const_iterator p = src.outputs_begin();
       p != src.outputs_end(); ++p) {
    BNode* src_node = *p;
    const char* src_name = src.node_name(src_node);
    BNode* dst_node = manip.new_output(src_name);
    ASSERT_COND( dst_node != 0 );
    node_assoc[src_node->id()] = dst_node;
  }

  // 中間ノード
  for (BNodeList::const_iterator p = src.logic_nodes_begin();
       p != src.logic_nodes_end(); ++p) {
    BNode* src_node = *p;
    const char* src_name = src.node_name(src_node);
    BNode* dst_node = manip.new_logic(src_name);
    ASSERT_COND( dst_node != 0 );
    node_assoc[src_node->id()] = dst_node;
  }

  // latch ノード
  for (BNodeList::const_iterator p = src.latch_nodes_begin();
       p != src.latch_nodes_end(); ++p) {
    BNode* src_node = *p;
    const char* src_name = src.node_name(src_node);
    BNode* dst_node = manip.new_latch(src_name);
    ASSERT_COND( dst_node != 0 );
    node_assoc[src_node->id()] = dst_node;
  }


  //////////////////////////////////////////////////////////////////////
  // PHASE-2: 内部情報をセットする．
  //////////////////////////////////////////////////////////////////////

  // 外部出力ノード
  for (BNodeList::const_iterator p = src.outputs_begin();
       p != src.outputs_end(); ++p) {
    BNode* src_node = *p;
    BNode* dst_node = node_assoc[src_node->id()];
    ASSERT_COND(dst_node != nullptr );

    BNode* src_fanin = src_node->fanin(0);
    BNode* dst_fanin = node_assoc[src_fanin->id()];
    ASSERT_COND(dst_fanin != nullptr );
    bool stat = manip.change_output(dst_node, dst_fanin);
    ASSERT_COND(stat );
  }

  // 中間ノード
  BNodeVector fanins;
  for (BNodeList::const_iterator p = src.logic_nodes_begin();
       p != src.logic_nodes_end(); ++p) {
    BNode* src_node = *p;
    BNode* dst_node = node_assoc[src_node->id()];
    ASSERT_COND(dst_node != nullptr );

    size_t n = src_node->fanin_num();
    fanins.clear();
    fanins.resize(n);
    for (size_t i = 0; i < n; i ++) {
      BNode* src_fanin = src_node->fanin(i);
      BNode* new_fanin = node_assoc[src_fanin->id()];
      ASSERT_COND( new_fanin != nullptr );
      fanins[i] = new_fanin;
    }
    bool stat = manip.change_logic(dst_node, src_node->func(), fanins, false);
    ASSERT_COND(stat );
  }

  // latch ノード
  for (BNodeList::const_iterator p = src.latch_nodes_begin();
       p != src.latch_nodes_end(); ++p) {
    BNode* src_node = *p;
    BNode* dst_node = node_assoc[src_node->id()];
    ASSERT_COND(dst_node != nullptr );

    BNode* src_fanin = src_node->fanin(0);
    BNode* dst_fanin = node_assoc[src_fanin->id()];
    ASSERT_COND(dst_fanin != nullptr );
    bool stat = manip.change_latch(dst_node, dst_fanin, src_node->reset_value());
    ASSERT_COND(stat );
  }
}

// @brief 全てのノードの論理式を簡単化する．
void
BNetwork::lexp_simplify()
{
  BNetManip manip(this);

  sweep();

  ymuint n = logic_node_num();
  BNodeVector node_list;
  tsort(node_list);
  for (ymuint i = 0; i < n; ++ i) {
    BNode* node = node_list[i];
    lexp_simplify_node(node);
    if ( node->fanin_num() <= 1 ) {
      manip.eliminate_node(node);
    }
  }

  sweep();
}

// @brief ノードの論理式を簡単化する．
// @param[in] node 対象のノード
void
BNetwork::lexp_simplify_node(BNode* node)
{
  if ( node->parent() != this ) {
    // node はこのネットワークに属している節点ではない
    BNET_ERROR("node is not a member of the network.");
    return;
  }

  Expr expr = node->func();
  expr.simplify();
  if ( expr.litnum() != node->func().litnum() ) {
    BNetManip manip(this);
    manip.change_logic(node, expr);
  }
}

// ファンアウト数が0のノードを削除する．
// ファンアウトしているノードがあってもそのノードのファンアウト数が
// 0の場合には，そのノードが削除されて自分のファンアウト数が0となって
// 自分が削除されることがあり得る．そのために削除は出力側から行う．
void
BNetwork::clean_up()
{
  // 削除対象のノードを入れるキュー(実際にはスタック)
  vector<BNode*> del_nodes;
  del_nodes.reserve(logic_node_num() + latch_node_num());

  // 現在，ファンアウト数が0のノードをdel_nodesに入れる．
  for (BNodeList::const_iterator p = logic_nodes_begin();
       p != logic_nodes_end(); ++p) {
    BNode* node = *p;
    if ( !node->fanout_num() ) {
      del_nodes.push_back(node);
    }
  }

  // キューが空になるまで削除を続ける．
  // 自分が削除された時にそのファンインのノード
  // のファンアウト数が0になればをキューに積む．
  while ( !del_nodes.empty() ) {
    BNode* node = del_nodes.back();
    del_nodes.pop_back();
    ymuint n = node->fanin_num();
    for (ymuint i = 0; i < n; i ++) {
      BNode* inode = node->fanin(i);
      if ( inode->is_input() ) continue;
      if ( inode->fanout_num() == 1 ) {
	ASSERT_COND( (*inode->fanouts_begin())->to() == node );
	// node を削除するとinodeのファンアウト数が0になるので，
	// キューに積む．
	del_nodes.push_back(inode);
      }
    }

    // このノードを削除する．
    delete_node(node);
  }
}

// 定数ノード/バッファ/インバータ/使われていないノードの消去を行う．
// 副作用としてファンアウト数が0のノードが削除される．
void
BNetwork::sweep()
{
  BNetManip manip(this);

  ymuint n_old = 0;
  ymuint n_new = logic_node_num();
  ymuint lit_old = 0;
  ymuint lit_new = litnum();
  while ( n_old != n_new || lit_old != lit_new ) {
    // ノードをトポロジカル順に array に入れる．
    BNodeVector node_list;
    tsort(node_list);

    // 入力数が 0 もしくは 1 のノードを消去する．
    // 一旦 node_list に移すのはループをうまく回すため．
    for (BNodeVector::const_iterator p = node_list.begin();
	 p != node_list.end(); ++ p) {
      BNode* node = *p;
      if ( node->fanin_num() == 0 || node->fanin_num() == 1 ) {
	manip.eliminate_node(node);
      }
    }

    // 不要になったノードを削除する．
    clean_up();

    // ノード数/リテラル数を更新する．
    n_old = n_new;
    n_new = logic_node_num();
    lit_old = lit_new;
    lit_new = litnum();
  }
}

// ファンアウト数が0の外部入力を削除する．
void
BNetwork::delete_unused_input()
{
  BNodeVector array;
  array.reserve(input_num());
  for (BNodeList::const_iterator p = inputs_begin();
       p != inputs_end(); ++p) {
    BNode* node = *p;
    if ( node->fanout_num() == 0 ) {
      // ここで node を削除してしまうとリストの構造が変わるので
      // for-loop が正しくまわらない．
      array.push_back(node);
    }
  }
  for (BNodeVector::iterator p = array.begin();
       p != array.end(); ++ p) {
    BNode* node = *p;
    bool stat = delete_node(node);
    ASSERT_COND( stat );
  }
}

// 型と名前を指定してノードを作る．
// エラーが起きた場合には nullptr を返す．
BNode*
BNetwork::new_node(BNode::tType type,
		   const char* name)
{
  // ID番号を取得する．
  int id = mItvlMgr.avail_num();
  if ( id == -1 ) {
    // 使用可能な ID 番号がない．！
    BNET_ERROR("no available ids");
    return nullptr;
  }
  mItvlMgr.erase(id);

  ymuint iid = static_cast<ymuint>(id);
  while ( mNodeVector.size() <= iid ) {
    BNode* node = BNodeMgr::the_obj().alloc_node();
    if ( !node ) {
      // なんらかのエラーが起きたので nullptr を返す．
      BNET_ERROR("Could not make a new node.");
      return nullptr;
    }
    node->mId = mNodeVector.size();
    node->mParent = this;
    mNodeVector.push_back(node);
    // 名前はダミー
    mNodeName.push_back(nullptr);
  }
  BNode* node = mNodeVector[iid];

  // タイプをセット
  // set_node_name() でタイプを見ているのでその前にセットする必要がある．
  node->set_type(type);

  // 名前をセット
  bool stat = set_node_name(node, name);
  if ( !stat ) {
    // 失敗，たぶん，名前が重複していた．
    mItvlMgr.add(id);
    BNET_ERROR("Could not register a new node.");
    return nullptr;
  }

  // PSet に登録する．
  switch ( type ) {
  case BNode::kPI:
    mPiList.push_back(node);
    break;
  case BNode::kPO:
    mPoList.push_back(node);
    break;
  case BNode::kLO:
    mLogicList.push_back(node);
    break;
  case BNode::kFF:
    mLatchList.push_back(node);
    break;
  default:
    // ここに来るはずはない．
    BNET_ERROR("illegal node type.");
    break;
  }
  mNodeList.push_back(node);

  mTraceMgr.prop_event(BNetChg::node_add(node->id()));

  // 構造が変わった
  structure_changed();

  // 正常終了
  return node;
}

// nodeで示される節点を削除する．
// エラーが起きたら false を返す．
// エラー条件に関してはBNetwork.hを参照のこと
bool
BNetwork::delete_node(BNode* node)
{
  if ( node->parent() != this ) {
    // 他のネットワークのノードなので削除できない．
    BNET_ERROR("node is not belong to the network.");
    return false;
  }
  if ( node->fanout_num() > 0 ) {
    // まだファンアウトがあるので削除できない
    BNET_ERROR("node still has fanouts.");
    return false;
  }

  // ファンインをクリアする．
  set_node_fanins(node, vector<BNode*>());

  // ID番号管理者にID番号を解放する．
  BNode::tId id = node->mId;
  mItvlMgr.add(id);

  // 名前辞書から削除
  reset_node_name(node);

  // 節点リスト(集合)から削除
  switch ( node->type() ) {
  case BNode::kPI:
    mPiList.remove(node);
    break;
  case BNode::kPO:
    mPoList.remove(node);
    break;
  case BNode::kLO:
    mLogicList.remove(node);
    break;
  case BNode::kFF:
    mLatchList.remove(node);
    break;
  default:
    ASSERT_NOT_REACHED;
  }
  mNodeList.remove(node);

  // 構造が変わったのでトポロジカルソートはやり直す必要がある．
  structure_changed();

  mTraceMgr.prop_event(BNetChg::node_delete(id));

  // 実際には node は mNodeVector[node->mId] に残ったまま
  // ただし ID そのものは回収されている

  // 正常終了
  return true;
}

// ノードの型を変える．
void
BNetwork::set_node_type(BNode* node,
			BNode::tType type)
{
  if ( node->parent() != this ) {
    // 他のネットワークのノードなのでエラー
    BNET_ERROR("node is not belong to the network.");
    return;
  }
  if ( node->type() == type ) {
    // 型は変わっていない．
    return;
  }

  // 現在属している節点リストからとりのぞく
  switch ( node->type() ) {
  case BNode::kPI:
    mPiList.remove(node);
    break;
  case BNode::kPO:
    mPoList.remove(node);
    break;
  case BNode::kLO:
    mLogicList.remove(node);
    break;
  case BNode::kFF:
    mLatchList.remove(node);
    break;
  default:
    ASSERT_NOT_REACHED;
  }

  node->set_type(type);

  // 新しい型の節点リストに加える．
  switch ( type ) {
  case BNode::kPI:
    mPiList.push_back(node);
    break;
  case BNode::kPO:
    mPoList.push_back(node);
    break;
  case BNode::kLO:
    mLogicList.push_back(node);
    break;
  case BNode::kFF:
    mLatchList.push_back(node);
    break;
  default:
    // ここに来るはずはない．
    BNET_ERROR("illegal node type.");
    break;
  }

  // 構造が変わった
  structure_changed();
}

// ノードに名前を設定して登録する．
// name が nullptr の場合には名前を自動生成する．
// 名前が重複していた場合には false を返す．
bool
BNetwork::set_node_name(BNode* node,
			const char* name)
{
  if ( name == nullptr || name[0] == '\0' ) {
    // 名前を適当につける．
    name = mNameMgr.new_name(true).c_str();
  }
  else {
    // 名前をセット
    mNameMgr.add(name);
  }
  // BNode 用に文字列領域を確保する．
  name = BNodeMgr::the_obj().alloc_name(name);
  mNodeName[node->id()] = name;

  if ( node->is_output() ) {
    // 連想配列に登録
    // と同時に重複していないかのチェック
    if ( !mPoMap->reg_name(name, node->id()) ) {
      // 同名の外部出力が既に存在する．
      return false;
    }
  }
  else {
    // 連想配列に登録
    // と同時に重複していないかのチェック
    if ( !mNameMap->reg_name(name, node->id()) ) {
      // 同名の外部入力または論理ノードが既に存在する．
      return false;
    }
  }

  // 正常終了
  return true;
}

// ノードの名前をクリアする．
void
BNetwork::reset_node_name(BNode* node)
{
  const char* name = node_name(node);
  if ( node->is_output() ) {
    // 名前辞書から削除
    mPoMap->del_name(name);
  }
  else {
    // 名前辞書から削除
    mNameMap->del_name(name);
    mNameMgr.erase(name);
  }
  BNodeMgr::the_obj().free_name(name);
}

// nodeで示される節点の名前を name に変更する．
// エラーが起きたら false を返す．
bool
BNetwork::rename_node(BNode* node,
		      const char* name)
{
  if ( node->parent() != this ) {
    // 他のネットワークのノードなので改名できない．
    BNET_ERROR("node is not belong to the network.");
    return false;
  }

  if ( name != nullptr &&
       ((node->is_output() && find_ponode(name)) ||
	(!node->is_output() && find_node(name))) ) {
    // name はもうすでに使われている．
    BNET_ERROR("name has been in use.");
    return false;
  }

  // 名前辞書から削除
  reset_node_name(node);

  // 新しい名前を付ける．
  set_node_name(node, name);

  mTraceMgr.prop_event(BNetChg::node_name(node->id()));

  // 正常終了
  return true;
}

// ノードのファンインをセットする．
void
BNetwork::set_node_fanins(BNode* node,
			  const BNodeVector& fanins)
{
  // 昔の接続を切る．
  ymuint old_ni = node->fanin_num();
  BNodeEdge* edge_array = node->mFaninEdgeArray;
  for (ymuint i = 0; i < old_ni; i ++) {
    BNodeEdge* edge = &edge_array[i];
    BNode* from = edge->from();
    change_edge_fanin(edge, from, nullptr);
    edge->mFrom = nullptr;
  }

  // 新しいファンイン用の配列を確保する．
  BNodeMgr& mgr = BNodeMgr::the_obj();
  ymuint new_ni = fanins.size();
  if ( old_ni != new_ni ) {
    mgr.free_edgearray(old_ni, edge_array);
    edge_array = mgr.alloc_edgearray(new_ni);
    node->mFaninEdgeArray = edge_array;
    node->flags_set_fanin_num(new_ni);
  }

  // 新しいファンインの情報を設定する．
  for (ymuint i = 0; i < new_ni; i ++) {
    BNodeEdge* edge = &edge_array[i];
    edge->mTo = node;
    edge->mPos = i;
    edge->mFrom = nullptr;
    change_edge_fanin(edge, nullptr, fanins[i]);
  }

  // ネットワークの構造が変わっているのでその印を付ける．
  mTraceMgr.prop_event(BNetChg::node_fanin(node->id()));
}

// 枝のファンイン元のノードを切り替える．
void
BNetwork::change_edge_fanin(BNodeEdge* edge,
			    BNode* old_node,
			    BNode* new_node)
{
  ASSERT_COND(edge->mFrom == old_node );
  if ( old_node ) {
    old_node->mFanouts.erase(edge);
  }
  if ( new_node ) {
    new_node->mFanouts.push_back(edge);
  }
  edge->mFrom = new_node;
  mTraceMgr.prop_event(BNetChg::node_fanin(edge->to()->id()));

  // 構造が変わった．
  structure_changed();
}

// ノードの論理式をセットする．
void
BNetwork::set_node_func(BNode* node,
			const Expr& lexp)
{
  if ( node->parent() != this ) {
    // 他のネットワークのノードなのでエラー
    BNET_ERROR("node is not belong to the network.");
    return;
  }
  node->mFunc = lexp;

  // node の論理式が変わった．
  mTraceMgr.prop_event(BNetChg::node_func(node->id()));
}

// ノードのリセット値をセットする．
// latch ノードの場合のみ意味を持つ．
void
BNetwork::set_node_reset_value(BNode* node,
			       int reset_value)
{
  if ( node->parent() != this ) {
    // 他のネットワークのノードなのでエラー
    BNET_ERROR("node is not belong to the network.");
    return;
  }

  node->flags_set_reset_value(reset_value);
}

// @brief 入力からのトポロジカルソート順を求める．
// @param[out] node_list 結果のノード配列
// @param[in] reverse true の時は出力からのトポロジカルソートを行う．
void
BNetwork::tsort(BNodeVector& node_list,
		bool reverse) const
{
  ymuint nv = logic_node_num();
  node_list.clear();
  node_list.reserve(nv);

  for (BNodeList::const_iterator p = inputs_begin();
       p != inputs_end(); ++p) {
    BNode* node = *p;
    node->flags_set_temp();
  }
  for (BNodeList::const_iterator p = latch_nodes_begin();
       p != latch_nodes_end(); ++ p) {
    BNode* node = *p;
    node->flags_set_temp();
  }

  for (BNodeList::const_iterator p = logic_nodes_begin();
       p != logic_nodes_end(); ++p) {
    BNode* node = *p;
    tsort_sub(node, node_list);
  }
  ASSERT_COND(node_list.size() == nv );

  for (BNodeList::const_iterator p = inputs_begin();
       p != inputs_end(); ++p) {
    BNode* node = *p;
    node->flags_clr_temp();
  }
  for (BNodeList::const_iterator p = latch_nodes_begin();
       p != latch_nodes_end(); ++ p) {
    BNode* node = *p;
    node->flags_clr_temp();
  }
  for (BNodeVector::iterator p = node_list.begin();
       p != node_list.end(); ++ p) {
    BNode* node = *p;
    node->flags_clr_temp();
  }

  if ( reverse ) {
    // node_list を逆順にならべる．
    ymuint i = 0;
    ymuint j = nv - 1;
    for ( ; i < j; ++ i, -- j) {
      BNode* node1 = node_list[i];
      BNode* node2 = node_list[j];
      node_list[i] = node2;
      node_list[j] = node1;
    }
  }
}

// 組み合わせ回路のループチェック
bool
BNetwork::acyclic_check() const
{
  // 中身はほとんど sort そのもの
  ymuint nv = logic_node_num();
  BNodeVector node_list;
  node_list.reserve(nv);

  for (BNodeList::const_iterator p = inputs_begin();
       p != inputs_end(); ++p) {
    BNode* node = *p;
    node->flags_set_temp();
  }
  for (BNodeList::const_iterator p = latch_nodes_begin();
       p != latch_nodes_end(); ++ p) {
    BNode* node = *p;
    node->flags_set_temp();
  }

  for (BNodeList::const_iterator p = logic_nodes_begin();
       p != logic_nodes_end(); ++p) {
    BNode* node = *p;
    tsort_sub(node, node_list);
  }
  bool ans = node_list.size() == nv;

  for (BNodeList::const_iterator p = inputs_begin();
       p != inputs_end(); ++p) {
    BNode* node = *p;
    node->flags_clr_temp();
  }
  for (BNodeList::const_iterator p = latch_nodes_begin();
       p != latch_nodes_end(); ++ p) {
    BNode* node = *p;
    node->flags_clr_temp();
  }
  for (BNodeVector::iterator p = node_list.begin();
       p != node_list.end(); ++ p) {
    BNode* node = *p;
    node->flags_clr_temp();
  }

  return ans;
}

// トポロジカルソートを行う．
// TSort のためのサブルーティン
void
BNetwork::tsort_sub(BNode* node,
		    BNodeVector& node_list) const
{
  if ( node->flags_get_temp() ) {
    // 既に配列に入っている．
    return;
  }
  if ( node->is_output() || node->is_latch() ) {
    // 外部出力の節点とラッチは対象外
    return;
  }

  ymuint n = node->fanin_num();
  for (ymuint i = 0; i < n; i ++) {
    BNode* inode = node->fanin(i);
    if ( !inode->flags_get_temp() ) {
      // まだ入力側に登録されていないものがある．
      return;
    }
  }

  node->flags_set_temp();
  node_list.push_back(node);

  for (BNodeFoList::const_iterator p = node->fanouts_begin();
       p != node->fanouts_end(); ++p) {
    BNodeEdge* edge = *p;
    BNode* onode = edge->to();
    tsort_sub(onode, node_list);
  }
}


//////////////////////////////////////////////////////////////////////
// BNetwork に変更があったことを通知するクラス
//////////////////////////////////////////////////////////////////////

// デストラクタ
// ネットワークにバインドしていたら解除する．
BNetworkTrace::~BNetworkTrace()
{
  unbind();
}

END_NAMESPACE_YM_NETWORKS_BNET
