﻿
/// @file BdnMgr.cc
/// @brief BdnMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "BdnMgrImpl.h"
#include "YmNetworks/BdnPort.h"
#include "YmNetworks/BdnDff.h"
#include "YmNetworks/BdnLatch.h"
#include "YmNetworks/BdnNode.h"
#include "YmNetworks/BdnNodeHandle.h"
#include "BdnAuxData.h"


BEGIN_NAMESPACE_YM_NETWORKS_BDN

///////////////////////////////////////////////////////////////////////
// クラス BdnMgrImpl
///////////////////////////////////////////////////////////////////////

// コンストラクタ
BdnMgrImpl::BdnMgrImpl() :
  mAlloc(4096),
  mHashTable(nullptr),
  mHashSize(0),
  mLevel(0U)
{
  alloc_table(1024);
}

// デストラクタ
BdnMgrImpl::~BdnMgrImpl()
{
  clear();
  delete [] mHashTable;
}

// 複製する．
void
BdnMgrImpl::copy(const BdnMgr& src)
{
  ymuint n = src.max_node_id();
  vector<BdnNode*> nodemap(n);

  // ポートの生成
  ymuint np = src.port_num();
  for (ymuint i = 0; i < np; ++ i) {
    const BdnPort* src_port = src.port(i);
    ymuint bw = src_port->bit_width();
    vector<ymuint> iovect(bw);
    src_port->get_iovect(iovect);
    BdnPort* dst_port = new_port(src_port->name().c_str(), iovect);
    for (ymuint j = 0; j < bw; ++ j) {
      const BdnNode* src_input = src_port->input(j);
      if ( src_input ) {
	nodemap[src_input->id()] = dst_port->_input(j);
      }
      const BdnNode* src_output = src_port->output(j);
      if ( src_output ) {
	nodemap[src_output->id()] = dst_port->_output(j);
      }
    }
  }

  // D-FFノードの生成
  const BdnDffList& src_dff_list = src.dff_list();
  for (BdnDffList::const_iterator p = src_dff_list.begin();
       p != src_dff_list.end(); ++ p) {
    const BdnDff* src_dff = *p;
    BdnDff* dst_dff = new_dff(src_dff->name().c_str());

    const BdnNode* src_output = src_dff->output();
    BdnNode* dst_output = dst_dff->_output();
    nodemap[src_output->id()] = dst_output;

    const BdnNode* src_input = src_dff->input();
    BdnNode* dst_input = dst_dff->_input();
    nodemap[src_input->id()] = dst_input;

    const BdnNode* src_clock = src_dff->clock();
    BdnNode* dst_clock = dst_dff->_clock();
    nodemap[src_clock->id()] = dst_clock;

    const BdnNode* src_clear = src_dff->clear();
    BdnNode* dst_clear = dst_dff->_clear();
    nodemap[src_clear->id()] = dst_clear;

    const BdnNode* src_preset = src_dff->preset();
    BdnNode* dst_preset = dst_dff->_preset();
    nodemap[src_preset->id()] = dst_preset;
  }

  // ラッチノードの生成
  const BdnLatchList& src_latch_list = src.latch_list();
  for (BdnLatchList::const_iterator p = src_latch_list.begin();
       p != src_latch_list.end(); ++ p) {
    const BdnLatch* src_latch = *p;
    BdnLatch* dst_latch = new_latch(src_latch->name().c_str());

    const BdnNode* src_output = src_latch->output();
    BdnNode* dst_output = dst_latch->_output();
    nodemap[src_output->id()] = dst_output;

    const BdnNode* src_input = src_latch->input();
    BdnNode* dst_input = dst_latch->_input();
    nodemap[src_input->id()] = dst_input;

    const BdnNode* src_enable = src_latch->enable();
    BdnNode* dst_enable = dst_latch->_enable();
    nodemap[src_enable->id()] = dst_enable;

    const BdnNode* src_clear = src_latch->clear();
    BdnNode* dst_clear = dst_latch->_clear();
    nodemap[src_clear->id()] = dst_clear;

    const BdnNode* src_preset = src_latch->preset();
    BdnNode* dst_preset = dst_latch->_preset();
    nodemap[src_preset->id()] = dst_preset;
  }

  // 論理ノードの生成
  vector<const BdnNode*> node_list;
  src.sort(node_list);
  ymuint nl = node_list.size();
  for (ymuint i = 0; i < nl; ++ i) {
    const BdnNode* src_node = node_list[i];

    const BdnNode* src_inode0 = src_node->fanin0();
    BdnNode* input0 = nodemap[src_inode0->id()];
    ASSERT_COND(input0 );
    bool inv0 = src_node->fanin0_inv();

    const BdnNode* src_inode1 = src_node->fanin1();
    BdnNode* input1 = nodemap[src_inode1->id()];
    ASSERT_COND(input1 );
    bool inv1 = src_node->fanin1_inv();

    BdnNodeHandle tmp_h = set_logic(nullptr, src_node->is_xor(),
				    BdnNodeHandle(input0, inv0),
				    BdnNodeHandle(input1, inv1));
    ASSERT_COND(tmp_h.inv() == false );
    BdnNode* dst_node = tmp_h.node();
    nodemap[src_node->id()] = dst_node;
  }

  // 外部出力ノードのファンインの設定
  const BdnNodeList& src_output_list = src.output_list();
  for (BdnNodeList::const_iterator p = src_output_list.begin();
       p != src_output_list.end(); ++ p) {
    const BdnNode* src_onode = *p;
    const BdnNode* src_inode = src_onode->output_fanin();
    BdnNode* dst_inode = nullptr;
    if ( src_inode ) {
      dst_inode = nodemap[src_inode->id()];
    }
    BdnNodeHandle dst_handle(dst_inode, src_onode->output_fanin_inv());
    BdnNode* dst_onode = nodemap[src_onode->id()];
    change_output_fanin(dst_onode, dst_handle);
  }
}

// 空にする．
void
BdnMgrImpl::clear()
{
  mName = string();
  mPortArray.clear();
  mDffItvlMgr.clear();
  mDffList.clear();
  mLatchItvlMgr.clear();
  mLatchList.clear();
  mNodeItvlMgr.clear();
  mInputList.clear();
  mOutputList.clear();
  mLnodeList.clear();
  mLevel = 0U;

  // mNodeArray, mDffArray, mLatchArray はクリアしない．
}

BEGIN_NONAMESPACE

// sort() の下請け関数
// node のファンアウトのうち，すべてのファンインがマークされている
// ノードを node_list に追加する．
void
sort_sub(const BdnNode* node,
	 vector<bool>& marks,
	 vector<const BdnNode*>& node_list)
{
  const BdnFanoutList& fo_list = node->fanout_list();
  for (BdnFanoutList::const_iterator p = fo_list.begin();
       p != fo_list.end(); ++ p) {
    const BdnEdge* e = *p;
    const BdnNode* onode = e->to();
    if ( !marks[onode->id()] && onode->is_logic() &&
	 marks[onode->fanin0()->id()] &&
	 marks[onode->fanin1()->id()] ) {
      marks[onode->id()] = true;
      node_list.push_back(onode);
    }
  }
}

END_NONAMESPACE

// @brief ソートされたノードのリストを得る．
void
BdnMgrImpl::sort(vector<const BdnNode*>& node_list) const
{
  node_list.clear();
  node_list.reserve(lnode_num());

  // 処理済みの印を表す配列
  ymuint n = max_node_id();
  vector<bool> marks(n, false);

  // 外部入力のみをファンインにするノードを node_list に追加する．
  for (BdnNodeList::const_iterator p = mInputList.begin();
       p != mInputList.end(); ++ p) {
    const BdnNode* node = *p;
    marks[node->id()] = true;
    sort_sub(node, marks, node_list);
  }
  // node_list のノードを一つずつとりだし，ファンアウトが node_list
  // に積めるかチェックする．
  for (ymuint rpos = 0; rpos < node_list.size(); ++ rpos) {
    const BdnNode* node = node_list[rpos];
    sort_sub(node, marks, node_list);
  }
  // うまくいっていれば全ての論理ノードが node_list に入っているはず．
  ASSERT_COND(node_list.size() == lnode_num() );
}

BEGIN_NONAMESPACE

// sort() の下請け関数
// node のファンアウトのうち，すべてのファンインがマークされている
// ノードを node_list に追加する．
void
sort_sub(BdnNode* node,
	 vector<bool>& marks,
	 vector<BdnNode*>& node_list)
{
  const BdnFanoutList& fo_list = node->fanout_list();
  for (BdnFanoutList::const_iterator p = fo_list.begin();
       p != fo_list.end(); ++ p) {
    BdnEdge* e = *p;
    BdnNode* onode = e->to();
    if ( !marks[onode->id()] && onode->is_logic() &&
	 marks[onode->fanin0()->id()] &&
	 marks[onode->fanin1()->id()] ) {
      marks[onode->id()] = true;
      node_list.push_back(onode);
    }
  }
}

END_NONAMESPACE

// @brief ソートされたノードのリストを得る．
void
BdnMgrImpl::_sort(vector<BdnNode*>& node_list)
{
  node_list.clear();
  node_list.reserve(lnode_num());

  // 処理済みの印を表す配列
  ymuint n = max_node_id();
  vector<bool> marks(n, false);

  // 外部入力のみをファンインにするノードを node_list に追加する．
  for (BdnNodeList::iterator p = mInputList.begin();
       p != mInputList.end(); ++ p) {
    BdnNode* node = *p;
    marks[node->id()] = true;
    sort_sub(node, marks, node_list);
  }
  // node_list のノードを一つずつとりだし，ファンアウトが node_list
  // に積めるかチェックする．
  for (ymuint rpos = 0; rpos < node_list.size(); ++ rpos) {
    BdnNode* node = node_list[rpos];
    sort_sub(node, marks, node_list);
  }
  // うまくいっていれば全ての論理ノードが node_list に入っているはず．
  ASSERT_COND(node_list.size() == lnode_num() );
}


BEGIN_NONAMESPACE

// rsort() の下請け関数
// node のすべてのファンアウトがマークされていたら node_list に積む．
void
rsort_sub(const BdnNode* node,
	  vector<bool>& marks,
	  vector<const BdnNode*>& node_list)
{
  if ( node == nullptr ||
       !node->is_logic() ||
       marks[node->id()] ) return;

  const BdnFanoutList& fo_list = node->fanout_list();
  for (BdnFanoutList::const_iterator p = fo_list.begin();
       p != fo_list.end(); ++ p) {
    const BdnEdge* e = *p;
    const BdnNode* onode = e->to();
    if ( !marks[onode->id()] ) {
      return;
    }
  }
  marks[node->id()] = true;
  node_list.push_back(node);
}

END_NONAMESPACE

// @brief 逆順でソートされたノードのリストを得る．
void
BdnMgrImpl::rsort(vector<const BdnNode*>& node_list) const
{
  node_list.clear();
  node_list.reserve(lnode_num());

  // 処理済みの印をあらわす配列
  ymuint n = max_node_id();
  vector<bool> marks(n, false);

  // 外部出力のみをファンアウトにするノードを node_list に追加する．
  for (BdnNodeList::const_iterator p = mOutputList.begin();
       p != mOutputList.end(); ++ p) {
    const BdnNode* node = *p;
    marks[node->id()] = true;
    const BdnNode* inode = node->output_fanin();
    rsort_sub(inode, marks, node_list);
  }

  // node_list からノードを取り出し，同様の処理を行う．
  for (ymuint rpos = 0; rpos < node_list.size(); ++ rpos) {
    const BdnNode* node = node_list[rpos];
    rsort_sub(node->fanin0(), marks, node_list);
    rsort_sub(node->fanin1(), marks, node_list);
  }
  // うまくいっていればすべての論理ノードが node_list に入っているはず．
  ASSERT_COND(node_list.size() == lnode_num() );
}

// @brief 名前を設定する．
void
BdnMgrImpl::set_name(const char* name)
{
  mName = name;
}

// @brief どこにもファンアウトしていないノードを削除する．
void
BdnMgrImpl::clean_up()
{
#warning "TODO: 未完"
}

// @brief ポートを作る．
// @param[in] name 名前
// @param[in] iovect ビットごとの方向を指定する配列
// @note iovect の要素の値の意味は以下の通り
// - 0 : なし
// - 1 : 入力のみ
// - 2 : 出力のみ
// - 3 : 入力と出力
BdnPort*
BdnMgrImpl::new_port(const char* name,
		     const vector<ymuint>& iovect)
{
  void* p = mAlloc.get_memory(sizeof(BdnPort));
  BdnPort* port = new (p) BdnPort();

  port->mId = mPortArray.size();
  mPortArray.push_back(port);

  port->mName = name;

  ymuint bit_width = iovect.size();
  port->mBitWidth = bit_width;

  void* q = mAlloc.get_memory(sizeof(BdnNode*) * bit_width);
  port->mInputArray = new (q) BdnNode*[bit_width];

  void* r = mAlloc.get_memory(sizeof(BdnNode*) * bit_width);
  port->mOutputArray = new (r) BdnNode*[bit_width];

  void* s = mAlloc.get_memory(sizeof(BdnAuxData*) * bit_width);
  port->mAuxDataArray = new (s) BdnAuxData*[bit_width];

  for (ymuint i = 0; i < bit_width; ++ i) {
    void* t = mAlloc.get_memory(sizeof(BdnPortData));
    BdnPortData* aux_data =  new (t) BdnPortData(port, i);
    port->mAuxDataArray[i] = aux_data;

    BdnNode* input = nullptr;
    BdnNode* output = nullptr;
    ymuint val = iovect[i];
    if ( val & 1U ) {
      input = alloc_node();
      input->set_input_type(BdnNode::kPRIMARY_INPUT);
      input->mAuxData = aux_data;
      mInputList.push_back(input);
    }
    if ( val & 2U ) {
      output = alloc_node();
      output->set_output_type(BdnNode::kPRIMARY_OUTPUT);
      output->mAuxData = aux_data;
      mOutputList.push_back(output);
    }
    port->mInputArray[i] = input;
    port->mOutputArray[i] = output;
  }

  return port;
}

// @brief D-FF を作る．
// @param[in] name 名前
// @return 生成されたD-FFを返す．
BdnDff*
BdnMgrImpl::new_dff(const char* name)
{
  // 空いているIDを探して配列へ登録
  int id = mDffItvlMgr.avail_num();
  ASSERT_COND( id >= 0 );
  mDffItvlMgr.erase(id);

  ymuint uid = static_cast<ymuint>(id);
  if ( mDffArray.size() == uid ) {
    void* p = mAlloc.get_memory(sizeof(BdnDff));
    BdnDff* dff = new (p) BdnDff;
    dff->mId = uid;
    void* q = mAlloc.get_memory(sizeof(BdnDffData));
    BdnDffData* data = new (q) BdnDffData(dff);
    dff->mAuxData = data;
    mDffArray.push_back(dff);
  }
  else {
    ASSERT_COND( mDffArray.size() > uid );
  }

  BdnDff* dff = mDffArray[uid];

  dff->mName = name;

  BdnNode* output = alloc_node();
  dff->mOutput = output;
  output->set_input_type(BdnNode::kDFF_OUTPUT);
  output->mAuxData = dff->mAuxData;
  mInputList.push_back(output);

  BdnNode* input = alloc_node();
  dff->mInput = input;
  input->set_output_type(BdnNode::kDFF_DATA);
  input->mAuxData = dff->mAuxData;
  mOutputList.push_back(input);

  BdnNode* clock = alloc_node();
  dff->mClock = clock;
  clock->set_output_type(BdnNode::kDFF_CLOCK);
  clock->mAuxData = dff->mAuxData;
  mOutputList.push_back(clock);

  BdnNode* clear = alloc_node();
  dff->mClear = clear;
  clear->set_output_type(BdnNode::kDFF_CLEAR);
  clear->mAuxData = dff->mAuxData;
  mOutputList.push_back(clear);

  BdnNode* preset = alloc_node();
  dff->mPreset = preset;
  preset->set_output_type(BdnNode::kDFF_PRESET);
  preset->mAuxData = dff->mAuxData;
  mOutputList.push_back(preset);

  mDffList.push_back(dff);

  return dff;
}

// @brief D-FF を削除する．
// @param[in] dff 削除対象の D-FF
void
BdnMgrImpl::delete_dff(BdnDff* dff)
{
  // new_dff() と逆の処理を行う．
  mDffItvlMgr.add(static_cast<int>(dff->id()));

  mDffList.erase(dff);

  delete_node(dff->_output());
  delete_node(dff->_input());
  delete_node(dff->_clock());
  delete_node(dff->_clear());
  delete_node(dff->_preset());
}

// @brief ラッチを作る．
// @param[in] name 名前
// @return 生成されたラッチを返す．
BdnLatch*
BdnMgrImpl::new_latch(const char* name)
{
  // 空いているIDを探して配列へ登録する．
  int id = mLatchItvlMgr.avail_num();
  ASSERT_COND( id >= 0 );
  mLatchItvlMgr.erase(id);

  ymuint uid = static_cast<ymuint>(id);
  if ( mLatchArray.size() == uid ) {
    void* p = mAlloc.get_memory(sizeof(BdnLatch));
    BdnLatch* latch = new (p) BdnLatch;
    latch->mId = uid;
    void* q = mAlloc.get_memory(sizeof(BdnLatchData));
    BdnAuxData* data = new (q) BdnLatchData(latch);
    latch->mAuxData = data;
    mLatchArray.push_back(latch);
  }
  else {
    ASSERT_COND( mLatchArray.size() > uid );
  }
  BdnLatch* latch = mLatchArray[uid];

  latch->mName = name;

  BdnNode* output = alloc_node();
  latch->mOutput = output;
  output->set_input_type(BdnNode::kLATCH_OUTPUT);
  output->mAuxData = latch->mAuxData;
  mInputList.push_back(output);

  BdnNode* input = alloc_node();
  latch->mInput = input;
  input->set_output_type(BdnNode::kLATCH_DATA);
  input->mAuxData = latch->mAuxData;
  mOutputList.push_back(input);

  BdnNode* enable = alloc_node();
  latch->mEnable = enable;
  enable->set_output_type(BdnNode::kLATCH_ENABLE);
  enable->mAuxData = latch->mAuxData;
  mOutputList.push_back(enable);

  BdnNode* clear = alloc_node();
  latch->mClear = clear;
  clear->set_output_type(BdnNode::kLATCH_CLEAR);
  clear->mAuxData = latch->mAuxData;
  mOutputList.push_back(clear);

  BdnNode* preset = alloc_node();
  latch->mPreset = preset;
  preset->set_output_type(BdnNode::kLATCH_PRESET);
  preset->mAuxData = latch->mAuxData;
  mOutputList.push_back(preset);

  mLatchList.push_back(latch);

  return latch;
}

// @brief ラッチを削除する．
// @param[in] latch 削除対象のラッチ
void
BdnMgrImpl::delete_latch(BdnLatch* latch)
{
  // new_latch() と逆の処理を行う．
  mLatchItvlMgr.add(static_cast<int>(latch->id()));

  mLatchList.erase(latch);

  delete_node(latch->_output());
  delete_node(latch->_input());
  delete_node(latch->_enable());
  delete_node(latch->_clear());
  delete_node(latch->_preset());
}

// @brief バランス木を作る．
// @param[in] node 根のノード
// @param[in] is_xor XOR の時 true にするフラグ(false なら AND)
// @param[in] iinv 入力の反転属性
// @param[in] start 開始位置
// @param[in] num 要素数
// @param[in] node_list 入力のノードのリスト
// @note node が nullptr の場合，新しいノードを確保する．
BdnNodeHandle
BdnMgrImpl::make_tree(BdnNode* node,
		      bool is_xor,
		      bool iinv,
		      ymuint start,
		      ymuint num,
		      const vector<BdnNodeHandle>& node_list)
{
  switch ( num ) {
  case 0:
    ASSERT_NOT_REACHED;

  case 1:
    if ( iinv ) {
      return ~node_list[start];
    }
    else {
      return node_list[start];
    }

  case 2:
    if ( iinv ) {
      return set_logic(node, is_xor, ~node_list[start], ~node_list[start + 1]);
    }
    else {
      return set_logic(node, is_xor, node_list[start], node_list[start + 1]);
    }

  default:
    break;
  }

  ymuint nh = num / 2;
  BdnNodeHandle l = make_tree(nullptr, is_xor, iinv, start, nh, node_list);
  BdnNodeHandle r = make_tree(nullptr, is_xor, iinv, start + nh, num - nh, node_list);
  return set_logic(node, is_xor, l, r);
}

// @brief 論理ノードの内容を変更する．
// @param[in] node 変更対象の論理ノード
// @param[in] new_handle 設定する新しいハンドル
// @note node のファンアウト先の情報を書き換える．
void
BdnMgrImpl::change_logic(BdnNode* node ,
			 BdnNodeHandle new_handle)
{
  if ( new_handle == BdnNodeHandle(node, false) ) {
    // 変化なし
    return;
  }

  const BdnFanoutList& fo_list = node->fanout_list();
  // ループ中で接続が変わる可能性があるので BdnEdge のリストをコピーする．
  vector<BdnEdge*> tmp_list;
  tmp_list.reserve(fo_list.size());
  for (BdnFanoutList::const_iterator p = fo_list.begin();
       p != fo_list.end(); ++ p) {
    BdnEdge* edge = *p;
    tmp_list.push_back(edge);
  }

  // node のファンアウト先の内容を変える．
  for (vector<BdnEdge*>::iterator p = tmp_list.begin();
       p != tmp_list.end(); ++ p) {
    BdnEdge* edge = *p;
    BdnNode* onode = edge->to();
    if ( onode->is_logic() ) {
      BdnNodeHandle inode1_handle(onode->fanin0());
      BdnNodeHandle inode2_handle(onode->fanin1());
      if ( edge->pos() == 0 ) {
	inode1_handle = new_handle;
      }
      else {
	inode2_handle = new_handle;
      }
      BdnNodeHandle new_oh = set_logic(onode, onode->is_xor(),
				       inode1_handle, inode2_handle);
      change_logic(onode, new_oh);
    }
    else if ( onode->is_output() ) {
      change_output_fanin(onode, new_handle);
    }
  }
}

// @brief 出力ノードの内容を変更する
// @param[in] 変更対象の出力ノード
// @param[in] inode 入力のノード
void
BdnMgrImpl::change_output_fanin(BdnNode* node,
				BdnNodeHandle inode_handle)
{
  if ( node->is_input() ) {
    // BdnNode::alt_node() と同じコードだが const がつかない．
    const BdnPort* port = node->port();
    ASSERT_COND( port != nullptr );
    node = port->mOutputArray[node->port_bitpos()];
    ASSERT_COND( node != nullptr );
  }
  ASSERT_COND( node->is_output() );

  bool inv = inode_handle.inv();
  node->set_output_fanin_inv(inv);

  BdnNode* inode = inode_handle.node();
  connect(inode, node, 0);
}


BEGIN_NONAMESPACE

// BdnNode のハッシュ関数
inline
ymuint
hash_func(ymuint fcode,
	  const BdnNode* node1,
	  const BdnNode* node2)
{
  return ((node1->id() * 3) + (node2->id() << 3)) ^ fcode;
}

END_NONAMESPACE


// @brief 論理ノードを探す．
// @param[in] is_xor XOR の時 true にするフラグ(false なら AND)
// @param[in] inode1_handle 1番めの入力ノード+極性
// @param[in] inode2_handle 2番めの入力ノード+極性
// @return 該当のノード+極性を表すハンドルを返す．
// @note 見つからなかったらエラーハンドルを返す．
BdnNodeHandle
BdnMgrImpl::find_logic(bool is_xor,
		       BdnNodeHandle inode1_handle,
		       BdnNodeHandle inode2_handle) const
{
  BdnNodeHandle onode_handle = is_trivial(is_xor, inode1_handle, inode2_handle);
  if ( !onode_handle.is_error() ) {
    return onode_handle;
  }

  BdnNode* inode1;
  BdnNode* inode2;
  bool oinv;
  ymuint fcode = cannonicalize(is_xor, inode1_handle, inode2_handle,
			       inode1, inode2, oinv);

  // 同じ構造を持つノードが既にないか調べる．
  BdnNode* node = find_node(fcode, inode1, inode2);
  if ( node ) {
    // 同じノードがあった．
    return BdnNodeHandle(node, oinv);
  }

  return BdnNodeHandle::make_error();
}

// @brief 論理ノードの内容を設定する．
// @param[in] node 設定するノード
// @param[in] is_xor XOR の時 true にするフラグ(false なら AND)
// @param[in] inode1_handle 1番めの入力ノード+極性
// @param[in] inode2_handle 2番めの入力ノード+極性
// @return ノード＋極性を返す．
// @note すでに構造的に同じノードがあればそれを返す．
// @note なければ node に設定する．
// @note node が nullptr の場合，新しいノードを確保する．
BdnNodeHandle
BdnMgrImpl::set_logic(BdnNode* node,
		      bool is_xor,
		      BdnNodeHandle inode1_handle,
		      BdnNodeHandle inode2_handle)
{
  BdnNodeHandle onode_handle = is_trivial(is_xor, inode1_handle, inode2_handle);
  if ( !onode_handle.is_error() ) {
    return onode_handle;
  }

  BdnNode* inode1;
  BdnNode* inode2;
  bool oinv;
  ymuint fcode = cannonicalize(is_xor, inode1_handle, inode2_handle,
			       inode1, inode2, oinv);

  // 同じ構造を持つノードが既にないか調べる．
  ymuint pos = hash_func(fcode, inode1, inode2);
  ymuint idx = pos % mHashSize;
  for (BdnNode* node1 = mHashTable[idx]; node1; node1 = node1->mLink) {
    if ( node1->_fcode() == fcode &&
	 node1->fanin0() == inode1 &&
	 node1->fanin1() == inode2 ) {
      // 同じノードがあった．
      return BdnNodeHandle(node1, oinv);
    }
  }

  if ( node == nullptr ) {
    // 新しいノードを作る．
    node = alloc_node();

    // 論理ノードリストに登録
    mLnodeList.push_back(node);

    if ( lnode_num() >= mNextLimit ) {
      alloc_table(mHashSize * 2);
      // サイズが変わったのでインデックスを再計算する．
      idx = pos % mHashSize;
    }
  }
  else {
    // ハッシュ表から取り除く
    ymuint pos0 = hash_func(node->_fcode(), node->fanin0(), node->fanin1());
    ymuint idx0 = pos0 % mHashSize;
    BdnNode* prev = mHashTable[idx0];
    if ( prev == node ) {
      mHashTable[idx0] = node->mLink;
    }
    for (BdnNode* node0 = 0; (node0 = prev->mLink); prev = node0) {
      if ( node0 == node ) {
	prev->mLink = node->mLink;
	break;
      }
    }
    // エラーチェック(node0 == nullptr) はしていない．
  }

  node->set_logic_type(fcode);
  connect(inode1, node, 0);
  connect(inode2, node, 1);

  // ハッシュ表に登録する．
  node->mLink = mHashTable[idx];
  mHashTable[idx] = node;

  return BdnNodeHandle(node, oinv);
}

// from を to の pos 番目のファンインとする．
// to の pos 番目にすでに接続があった場合には自動的に削除される．
void
BdnMgrImpl::connect(BdnNode* from,
		    BdnNode* to,
		    ymuint pos)
{
  // BdnNode::mFaoutList を変更するのはここだけ

  BdnEdge* edge = &(to->mFanins[pos]);
  BdnNode* old_from = edge->from();
  if ( old_from ) {
    old_from->mFanoutList.erase(edge);
    old_from->scan_po();
  }
  edge->set_from(from);
  if ( from ) {
    from->mFanoutList.push_back(edge);
    from->scan_po();
  }

  // 構造が変わったのでレベルの情報は無効化される．
  mLevel = 0U;
}

// @brief 論理ノードの自明な簡単化を行う．
// @param[in] is_xor XOR の時 true にするフラグ(false なら AND)
// @param[in] inode1_handle 1番めの入力ノード+極性
// @param[in] inode2_handle 2番めの入力ノード+極性
// @return 該当のノード+極性を表すハンドルを返す．
// @note 見つからなかったらエラーハンドルを返す．
BdnNodeHandle
BdnMgrImpl::is_trivial(bool is_xor,
		       BdnNodeHandle inode1_handle,
		       BdnNodeHandle inode2_handle) const
{
  // 境界条件の検査
  if ( is_xor ) {
    // XOR の場合
    if ( inode1_handle.is_zero() ) {
      // 入力0が定数0だった．
      return inode2_handle;
    }
    else if ( inode1_handle.is_one() ) {
      // 入力0が定数1だった．
      return ~inode2_handle;
    }
    else if ( inode2_handle.is_zero() ) {
      // 入力1が定数0だった．
      return inode1_handle;
    }
    else if ( inode2_handle.is_one() ) {
      // 入力1が定数1だった．
      return ~inode1_handle;
    }
    else if ( inode1_handle == inode2_handle ) {
      // 2つの入力が同一だった．
      return inode1_handle;
    }
    else if ( inode1_handle == ~inode2_handle ) {
      // 2つの入力が極性違いだった．
      return BdnNodeHandle::make_zero();
    }
  }
  else {
    // AND の場合
    if ( inode1_handle.is_one() ) {
      // 入力0が定数1だった．
      return inode2_handle;
    }
    else if ( inode1_handle.is_zero() ) {
      // 入力0が定数0だった．
      return BdnNodeHandle::make_zero();
    }
    else if ( inode2_handle.is_one() ) {
      // 入力1が定数1だった．
      return inode1_handle;
    }
    else if ( inode2_handle.is_zero() ) {
      // 入力1が定数0だった．
      return BdnNodeHandle::make_zero();
    }
    else if ( inode1_handle == inode2_handle ) {
      // 2つの入力が同じだった．
      return inode1_handle;
    }
    else if ( inode1_handle == ~inode2_handle ) {
      // 2つの入力が極性違いだった．
      return BdnNodeHandle::make_zero();
    }
  }

  // ここまで来たら自明な簡単化は行えなかったということ．
  return BdnNodeHandle::make_error();
}

// @brief 論理ノードに設定する情報の正規化を行う．
// @param[in] is_xor XOR の時 true にするフラグ(false なら AND)
// @param[in] inode1_handle 1番めの入力ノード+極性
// @param[in] inode2_handle 2番めの入力ノード+極性
// @param[out] inode1 正規化された1番めの入力ノード
// @param[out] inode2 正規化された2番目の入力ノード
// @param[out] oinv 出力極性
// @return 機能コードを返す．
// @note 機能コードの各ビットの意味は以下のとおり，
//  - 0bit: ファンイン0の反転属性
//  - 1bit: ファンイン1の反転属性
//  - 2bit: XOR/AND フラグ( 0: AND, 1: XOR)
ymuint
BdnMgrImpl::cannonicalize(bool is_xor,
			  BdnNodeHandle inode1_handle,
			  BdnNodeHandle inode2_handle,
			  BdnNode*& inode1,
			  BdnNode*& inode2,
			  bool& oinv) const
{
  // 入力の反転属性
  bool inv1 = inode1_handle.inv();
  bool inv2 = inode2_handle.inv();

  // 入力のノード
  inode1 = inode1_handle.node();
  inode2 = inode2_handle.node();

  // ファンインの順番の正規化
  if ( inode1->id() > inode2->id() ) {
    BdnNode* tmp = inode1;
    inode1 = inode2;
    inode2 = tmp;
    bool tmp_inv = inv1;
    inv1 = inv2;
    inv2 = tmp_inv;
  }

  // 出力の反転属性
  oinv = false;

  // fcode の正規化
  if ( is_xor ) {
    // XOR の場合には入力に反転属性はつかない．
    oinv = inv1 ^ inv2;
    return XOR_BIT;
  }
  else {
    return static_cast<ymuint>(inv1) | (static_cast<ymuint>(inv2) << 1);
  }
}

// @brief ノードを探索する．
// @param[in] fcode 機能コード
// @param[in] inode1 1番めのノード
// @param[in] inode2 2番めのノード
BdnNode*
BdnMgrImpl::find_node(ymuint fcode,
		      const BdnNode* inode1,
		      const BdnNode* inode2) const
{
  ymuint idx = hash_func(fcode, inode1, inode2) % mHashSize;
  for (BdnNode* node1 = mHashTable[idx]; node1; node1 = node1->mLink) {
    if ( node1->_fcode() == fcode &&
	 node1->fanin0() == inode1 &&
	 node1->fanin1() == inode2 ) {
      // 同じノードがあった．
      return node1;
    }
  }
  // 見つからなかった．
  return nullptr;
}

// @brief ノードを作成する．
// @return 作成されたノードを返す．
BdnNode*
BdnMgrImpl::alloc_node()
{
  // 空いているIDを探してノード配列へ登録
  int id = mNodeItvlMgr.avail_num();
  ASSERT_COND( id >= 0 );
  mNodeItvlMgr.erase(id);

  ymuint uid = static_cast<ymuint>(id);
  if ( mNodeArray.size() == uid ) {
    void* p = mAlloc.get_memory(sizeof(BdnNode));
    BdnNode* node = new (p) BdnNode();
    node->mId = uid;
    mNodeArray.push_back(node);
  }
  else {
    ASSERT_COND( mNodeArray.size() > uid );
  }
  BdnNode* node = mNodeArray[uid];

  node->mFlags = 0U;
  node->mAuxData = nullptr;

  return node;
}

// node を削除する．
void
BdnMgrImpl::delete_node(BdnNode* node)
{
  // alloc_node の逆の処理を行なう．
  mNodeItvlMgr.add(static_cast<int>(node->id()));

  if ( node->is_logic() ) {
    mLnodeList.erase(node);
  }
  else if ( node->is_input() ) {
    mInputList.erase(node);
  }
  else if ( node->is_output() ) {
    mOutputList.erase(node);
  }

  // mNodeArray 内のエントリはクリアしない．
  // id の再利用と同様に BdnNode も再利用する．
}

// @brief 段数を求める．
ymuint
BdnMgrImpl::level() const
{
  if ( (mLevel & 1U) == 0U ) {
    for (BdnNodeList::const_iterator p = mInputList.begin();
	 p != mInputList.end(); ++ p) {
      BdnNode* node = *p;
      node->mLevel = 0U;
    }

    vector<const BdnNode*> node_list;
    sort(node_list);
    for (vector<const BdnNode*>::const_iterator p = node_list.begin();
	 p != node_list.end(); ++ p) {
      const BdnNode* node = *p;
      const BdnNode* inode0 = node->fanin0();
      ymuint l = inode0->mLevel;
      const BdnNode* inode1 = node->fanin1();
      ymuint l1 = inode1->mLevel;
      if ( l < l1 ) {
	l = l1;
      }
      node->mLevel = l + 1;
    }

    ymuint max_l = 0;
    for (BdnNodeList::const_iterator p = mOutputList.begin();
	 p != mOutputList.end(); ++ p) {
      const BdnNode* node = *p;
      const BdnNode* inode = node->fanin0();
      if ( inode ) {
	ymuint l1 = inode->mLevel;
	if ( max_l < l1 ) {
	  max_l = l1;
	}
      }
    }
    mLevel = (max_l << 1) | 1U;
  }
  return (mLevel >> 1);
}

// @brief ハッシュ表を確保する．
void
BdnMgrImpl::alloc_table(ymuint req_size)
{
  BdnNode** old_table = mHashTable;
  ymuint old_size = mHashSize;

  if ( mHashSize == 0 ) {
    mHashSize = 1024;
  }
  while ( mHashSize < req_size ) {
    mHashSize <<= 1;
  }
  mHashTable = new BdnNode*[mHashSize];
  for (ymuint i = 0; i < mHashSize; ++ i) {
    mHashTable[i] = nullptr;
  }
  if ( old_size > 0 ) {
    for (ymuint i = 0; i < old_size; ++ i) {
      BdnNode* next = nullptr;
      for (BdnNode* node = old_table[i]; node; node = next) {
	next = node->mLink;
	ymuint pos = hash_func(node->_fcode(), node->fanin0(), node->fanin1());
	ymuint idx = pos % mHashSize;
	node->mLink = mHashTable[idx];
	mHashTable[idx] = node;
      }
    }
    delete [] old_table;
  }
  mNextLimit = static_cast<ymuint32>(mHashSize * 1.8);
}

END_NAMESPACE_YM_NETWORKS_BDN
