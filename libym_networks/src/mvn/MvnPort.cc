
/// @file MvnPort.cc
/// @brief MvnPort の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014, 2015 Yusuke Matsunaga
/// All rights reserved.


#include "YmNetworks/MvnPort.h"
#include "YmNetworks/MvnNode.h"


BEGIN_NAMESPACE_YM_NETWORKS_MVN

//////////////////////////////////////////////////////////////////////
// クラス MvnPortRef
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MvnPortRef::MvnPortRef()
{
}

// @brief ノード単体のポート要素用のコンストラクタ
// @param[in] node ノード
MvnPortRef::MvnPortRef(const MvnNode* node)
{
  mNode = node;
  mMsb = 0U;
  mLsb = 0U;
}

// @brief ビット指定のポート要素用のコンストラクタ
// @param[in] node ノード
// @param[in] bitpos ビット位置
MvnPortRef::MvnPortRef(const MvnNode* node,
		       ymuint bitpos)
{
  mNode = node;
  mMsb = (bitpos << 1) | 1U;
  mLsb = 0U;
}

// @brief 範囲指定のポート要素用のコンストラクタ
// @param[in] node ノード
// @param[in] msb 範囲指定の MSB
// @param[in] lsb 範囲指定の LSB
MvnPortRef::MvnPortRef(const MvnNode* node,
		       ymuint msb,
		       ymuint lsb)
{
  mNode = node;
  mMsb = (msb << 1);
  mLsb = (lsb << 1) | 1U;
}

// @brief デストラクタ
MvnPortRef::~MvnPortRef()
{
}

// @brief この実体のビット幅を返す．
ymuint
MvnPortRef::bit_width() const
{
  if ( has_bitselect() ) {
    return 1;
  }
  else if ( has_partselect() ) {
    return msb() - lsb() + 1;
  }
  else {
    const MvnNode* n = node();
    if ( n->type() == MvnNode::kInput ) {
      return n->bit_width();
    }
    else if ( n->type() == MvnNode::kOutput ) {
      return n->input(0)->bit_width();
    }
    else if ( n->type() == MvnNode::kInout ) {
      return n->bit_width();
    }
  }
  ASSERT_NOT_REACHED;
  return 0;
}


//////////////////////////////////////////////////////////////////////
// クラス MvnPort
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] portref_list ポート参照式のリスト
// @param[in] name 名前
//
// 名前がない場合(name == nullptr)もありうる．
// name == "" の場合も考慮する．
MvnPort::MvnPort(const vector<MvnPortRef>& portref_list,
		 const char* name)
{
  if ( name == nullptr || name[0] == '\0' ) {
    mName = string();
  }
  else {
    mName = string(name);
  }

  mPortRefNum = portref_list.size();
  mPortRefArray = new MvnPortRef[mPortRefNum];
  for (ymuint i = 0; i < mPortRefNum; ++ i) {
    mPortRefArray[i] = portref_list[i];
  }
}

// @brief デストラクタ
MvnPort::~MvnPort()
{
  delete [] mPortRefArray;
}

// @brief ビット幅を得る．
ymuint
MvnPort::bit_width() const
{
  ymuint ans = 0;
  for (ymuint i = 0; i < mPortRefNum; ++ i) {
    MvnPortRef& pr = mPortRefArray[i];
    ans += pr.bit_width();
  }
  return ans;
}

END_NAMESPACE_YM_NETWORKS_MVN
