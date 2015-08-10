﻿#ifndef NETWORKS_CMNNODE_H
#define NETWORKS_CMNNODE_H

/// @file YmNetworks/CmnNode.h
/// @brief CmnNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmNetworks/cmn.h"
#include "YmCell/cell_nsdef.h"

#include "YmUtils/DlList.h"


BEGIN_NAMESPACE_YM_NETWORKS_CMN

//////////////////////////////////////////////////////////////////////
/// @class CmnNode CmnNode.h "YmNetworks/CmnNode.h"
/// @brief セルネットワークを構成するノードを表すクラス
///
/// ノードの種類は
/// - 入力ノード
/// - 出力ノード
/// - 論理ノード
/// の 3種類がある．
///
/// ただし，入力ノードは以下の様に細分化される．
/// - 外部入力
/// - D-FF の肯定出力
/// - D-FF の否定出力
/// - ラッチの肯定出力
/// - ラッチの否定出力
///
/// 出力ノードは以下の様に細分化される．
/// - 外部出力
/// - D-FF のデータ入力
/// - D-FF のクロック
/// - D-FF のクリア信号
/// - D-FF のプリセット信号
/// - ラッチのデータ入力
/// - ラッチのイネーブル信号
/// - ラッチのクリア信号
/// - ラッチのプリセット信号
///
/// @sa CmnEdge CmnMgr
//////////////////////////////////////////////////////////////////////
class CmnNode :
  public DlElem
{
  friend class CmnMgrImpl;

public:

  /// @brief ノードの型
  enum tType {
    /// @brief 論理ノード
    kLOGIC        = 0,
    /// @brief 外部入力ノード
    kINPUT        = 1,
    /// @brief 外部出力ノード
    kOUTPUT       = 2,
  };

  /// @brief 入力ノードのサブタイプ
  enum tInputType {
    /// @brief 外部入力
    kPRIMARY_INPUT  = 0,
    /// @brief DFFの肯定出力
    kDFF_OUTPUT1    = 1,
    /// @brief DFFの否定出力
    kDFF_OUTPUT2    = 2,
    /// @brief ラッチの肯定出力
    kLATCH_OUTPUT1  = 3,
    /// @brief ラッチの否定出力
    kLATCH_OUTPUT2  = 4
  };

  /// @brief 出力ノードのサブタイプ
  enum tOutputType {
    /// @brief 外部出力
    kPRIMARY_OUTPUT = 0,
    /// @brief DFFのデータ
    kDFF_DATA       = 1,
    /// @brief DFFのクロック
    kDFF_CLOCK      = 2,
    /// @brief DFFのクリア信号
    kDFF_CLEAR      = 3,
    /// @brief DFFのプリセット信号
    kDFF_PRESET     = 4,
    /// @brief ラッチのデータ
    kLATCH_DATA     = 5,
    /// @brief ラッチのイネーブル
    kLATCH_ENABLE   = 6,
    /// @brief ラッチのクリア信号
    kLATCH_CLEAR    = 7,
    /// @brief ラッチのプリセット信号
    kLATCH_PRESET   = 8
  };


protected:

  /// @brief コンストラクタ
  CmnNode();

  /// @brief デストラクタ
  virtual
  ~CmnNode();


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 全てのタイプで共通な情報を取り出す関数
  /// @{

  /// @brief ID 番号の取得
  /// @return ID 番号を返す．
  /// @note ID 番号はノードの生成時に CmnMgr により自動的に割り振られる．
  /// @sa CmnMgr
  ymuint
  id() const;

  /// @brief ID を表す文字列の取得
  /// @note デバッグ時にしか意味を持たない
  string
  id_str() const;

  /// @brief タイプを得る．
  virtual
  tType
  type() const = 0;

  /// @brief 入力ノードの時に true を返す．
  virtual
  bool
  is_input() const;

  /// @brief 出力ノードの時に true を返す．
  virtual
  bool
  is_output() const;

  /// @brief 論理ノードの時に true を返す．
  virtual
  bool
  is_logic() const;

  /// @brief ファンイン数を得る．
  virtual
  ymuint
  fanin_num() const;

  /// @brief ファンインのノードを得る．
  /// @param[in] pos 入力番号 ( 0 <= pos < fanin_num() )
  /// @return pos 番めのファンインのノード
  /// @note 該当するファンインがなければ nullptr を返す．
  virtual
  const CmnNode*
  fanin(ymuint pos) const;

  /// @brief ファンインの枝を得る．
  /// @param[in] pos 入力番号
  /// @return pos 番目の入力の枝
  /// @note 該当するファンインの枝がなければ nullptr を返す．
  virtual
  const CmnEdge*
  fanin_edge(ymuint pos) const;

  /// @brief ファンインの枝を得る．
  /// @param[in] pos 入力番号
  /// @return pos 番目の入力の枝
  /// @note 該当するファンインの枝がなければ nullptr を返す．
  virtual
  CmnEdge*
  _fanin_edge(ymuint pos);

  /// @brief ファンアウト数を得る．
  ymuint
  fanout_num() const;

  /// @brief ファンアウトリストを得る．
  const CmnFanoutList&
  fanout_list() const;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 入力ノードおよび出力ノードに関係した情報を取り出す関数
  /// @{

  /// @brief 入力ノードのサブタイプを得る．
  virtual
  tInputType
  input_type() const;

  /// @brief 出力ノードのサブタイプを得る．
  virtual
  tOutputType
  output_type() const;

  /// @brief 関連するポートを返す．
  /// @note kPRIMARY_INPUT および kPRIMARY_OUTPUT の時のみ意味を持つ．
  /// @note それ以外では nullptr を返す．
  virtual
  const CmnPort*
  port() const;

  /// @brief ポート中のビット位置を返す．
  /// @note kPRIMARY_INPUT および kPRIMARY_OUTPUT の時のみ意味を持つ．
  /// @note それ以外では 0 を返す．
  virtual
  ymuint
  port_bitpos() const;

  /// @brief 入出力ノードの場合に相方のノードを返す．
  /// @note なければ nullptr を返す．
  virtual
  const CmnNode*
  alt_node() const;

  /// @brief 関連する D-FF を返す．
  /// @note D-FF に関連していない場合には nullptr を返す．
  virtual
  const CmnDff*
  dff() const;

  /// @brief 関連するラッチを返す．
  /// @note ラッチに関連していない場合には nullptr を返す．
  virtual
  const CmnLatch*
  latch() const;

  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 論理ノードの情報を取り出す関数
  /// @{

  /// @brief セルを得る．
  virtual
  const Cell*
  cell() const;

  /// @}
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  ymuint32 mId;

  // ファンアウトの枝のリスト
  CmnFanoutList mFanoutList;

};


//////////////////////////////////////////////////////////////////////
// inline 関数の定義
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// クラス CmnNode
//////////////////////////////////////////////////////////////////////

// ID 番号を得る．
inline
ymuint
CmnNode::id() const
{
  return mId;
}

// ファンアウト数を得る．
inline
ymuint
CmnNode::fanout_num() const
{
  return mFanoutList.size();
}

// ファンアウトリストを得る．
inline
const CmnFanoutList&
CmnNode::fanout_list() const
{
  return mFanoutList;
}

END_NAMESPACE_YM_NETWORKS_CMN

#endif // NETWORKS_CMNNODE_H
