#ifndef FSIM2_H
#define FSIM2_H

/// @file src/fsim2/Fsim2.h
/// @brief Fsim2 のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012 Yusuke Matsunaga
/// All rights reserved.


#include "fsim2_nsdef.h"
#include "Fsim.h"
#include "PackedVal.h"
#include "EventQ.h"
#include "FsimFault.h"
#include "ym_logic/LogExpr.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM2

class SimFFR;
class SimNode;

//////////////////////////////////////////////////////////////////////
/// @class Fsim2 Fsim2.h "Fsim2.h"
/// @brief 故障シミュレーションを行うモジュール
//////////////////////////////////////////////////////////////////////
class Fsim2 :
  public Fsim
{
public:

  /// @brief コンストラクタ
  Fsim2();

  /// @brief デストラクタ
  virtual
  ~Fsim2();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワークをセットする．
  virtual
  void
  set_network(const TgNetwork& network,
	      const vector<SaFault*>& flist);

  /// @brief 故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @param[out] det_faults 検出された故障を格納するリスト
  virtual
  void
  run(TestVector* tv,
      vector<SaFault*>& det_faults);

  /// @brief 故障シミュレーションを行う．
  /// @param[in] tv_array テストベクタの配列
  /// @param[out] det_faults 検出された故障を格納するリストの配列
  virtual
  void
  run(const vector<TestVector*>& tv_array,
      vector<vector<SaFault*> >& det_faults);

  /// @brief 一つのパタンで一つの故障に対するシミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @param[in] f 対象の故障
  virtual
  bool
  run(TestVector* tv,
      SaFault* f);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内部の故障リストの更新を行なう．
  void
  update_faults();

  /// @brief FFR 内の故障シミュレーションを行う．
  PackedVal
  ffr_simulate(SimFFR* ffr);

  /// @brief イベントキューを用いてシミュレーションを行う．
  PackedVal
  eventq_simulate();

  /// @brief ffr 内の故障が検出可能か調べる．
  void
  fault_sweep(SimFFR* ffr,
	      vector<SaFault*>& det_faults);


private:
  //////////////////////////////////////////////////////////////////////
  // TgNode と SimNode の対応関係を表すためのデータ構造
  //////////////////////////////////////////////////////////////////////

  struct EdgeMap {
    SimNode* mNode;
    ymuint32 mPos;
  };


private:

  /// @brief 現在保持している SimNode のネットワークを破棄する．
  /// 内部で clear_faults() を呼ぶ．
  void
  clear();

  /// @brief FsimFault を破棄する．
  void
  clear_faults();

  /// @brief node に対応する SimNode を得る．
  SimNode*
  find_simnode(const TgNode* node) const;

  /// @brief node の pos 番めの入力に対応する枝を得る．
  void
  find_simedge(const TgNode* node,
	       ymuint pos,
	       SimNode*& simnode,
	       ymuint& ipos) const;

  /// @brief logic ノードを作る．
  SimNode*
  make_logic(const LogExpr& lexp,
	     const vector<SimNode*>& inputs,
	     const vector<EdgeMap*>& emap);

  /// @brief logic ノードを作る．
  SimNode*
  make_node(tTgGateType type,
	    const vector<SimNode*>& inputs);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TgNetwork* mNetwork;

  // TgNode の gid をキーにして SimNode を入れる配列
  vector<SimNode*> mSimMap;

  // TgNode の gid と入力番号から枝の情報を取り出す配列
  vector<vector<EdgeMap> > mEdgeMap;

  // 全ての SimNode を納めた配列
  vector<SimNode*> mNodeArray;

  // 外部入力に対応する SimNode を納めた配列
  vector<SimNode*> mInputArray;

  // 外部出力に対応する SimNode を納めた配列
  vector<SimNode*> mOutputArray;

  // 入力からのトポロジカル順に並べた logic ノードの配列
  vector<SimNode*> mLogicArray;

  // FFR を納めた配列
  vector<SimFFR> mFFRArray;

  // イベントキュー
  EventQ mEventQ;

  // 値を消去する必要のあるノードを入れておく配列
  vector<SimNode*> mClearArray;

  // 故障シミュレーション用の故障の配列
  vector<FsimFault> mFsimFaults;

};

END_NAMESPACE_YM_SATPG_FSIM2

#endif // FSIM2_H