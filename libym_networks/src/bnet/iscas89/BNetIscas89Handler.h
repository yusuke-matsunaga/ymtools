﻿#ifndef BNETISCAS89HANDLER_H
#define BNETISCAS89HANDLER_H

/// @file BNetIscas89Handler.h
/// @brief BNetIscas89Handler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmNetworks/bnet.h"
#include "Iscas89Handler.h"


BEGIN_NAMESPACE_YM_ISCAS89

//////////////////////////////////////////////////////////////////////
/// @class BNetIscas89Handler BNetIscas89Handler.h "BNetIscas89Handler.h"
/// @brief BNetwork 用の Iscas89Handler
//////////////////////////////////////////////////////////////////////
class BNetIscas89Handler :
  public Iscas89Handler
{
public:

  /// @brief コンストラクタ
  BNetIscas89Handler();

  /// @brief デストラクタ
  virtual
  ~BNetIscas89Handler();


public:

  /// @brief 読み込む対象のネットワークを設定する．
  void
  set_network(BNetwork* network);

  /// @brief 初期化
  virtual
  bool
  init();

  /// @brief INPUT 文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] name_id 入力ピン名の ID 番号
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  read_input(const FileRegion& loc,
	     ymuint32 name_id);

  /// @brief OUTPUT 文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] name_id 出力ピン名の ID 番号
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  read_output(const FileRegion& loc,
	      ymuint32 name_id);

  /// @brief ゲート文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] type ゲートの型
  /// @param[in] oname_id 出力名の ID 番号
  /// @param[in] iname_list 入力名のリスト
  /// @retval true 処理が成功した．
  /// @retval false エラーが起こった．
  virtual
  bool
  read_gate(const FileRegion& loc,
	    tIscas89GateType type,
	    ymuint32 oname,
	    const vector<ymuint32>& iname_list);

  /// @brief 通常終了時の処理
  virtual
  void
  normal_exit();

  /// @brief エラー終了時の処理
  virtual
  void
  error_exit();


private:

  struct NodeInfo
  {
    NodeInfo(ymuint32 id) : mId(id), mNode(nullptr) { }

    ymuint32 mId;
    BNode* mNode;
  };


private:

  /// @brief ノードの取得 & 生成
  BNode*
  get_node(ymuint32 id);

  /// @brief mNodeArray を確保する．
  void
  resize(ymuint32 id);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の BNetwork
  BNetwork* mNetwork;

  // ネットワークを操作するクラス
  BNetManip* mManip;

  // ゲート文のファンインの配列
  vector<BNode*> mCurFanins;

  // ID番号をキーにして NodeInfo を納めた配列
  vector<NodeInfo> mNodeArray;

};

END_NAMESPACE_YM_ISCAS89

#endif // BNETISCAS89HANDLER_H
