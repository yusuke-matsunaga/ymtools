#ifndef DTPGSAT_H
#define DTPGSAT_H

/// @file DtpgSat.h
/// @brief DtpgSat のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012 Yusuke Matsunaga
/// All rights reserved.


#include "Dtpg.h"
#include "TpgFault.h"
#include "SatEngine.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DtpgSat DtpgSat.h "DtpgSat.h"
/// @brief SAT を用いたパタン生成器
//////////////////////////////////////////////////////////////////////
class DtpgSat :
  public Dtpg
{
public:

  /// @brief コンストラクタ
  DtpgSat();

  /// @brief デストラクタ
  virtual
  ~DtpgSat();


public:
  //////////////////////////////////////////////////////////////////////
  // Dtpg の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 使用する SAT エンジンを指定する．
  virtual
  void
  set_mode(const string& type = string(),
	   const string& option = string(),
	   ostream* outp = NULL);

  /// @brief 回路と故障リストを設定する．
  /// @param[in] tpgnetwork 対象のネットワーク
  virtual
  void
  set_network(TpgNetwork& tgnetwork);

  /// @brief テスト生成を行なう．
  /// @param[in] mode メインモード
  /// @param[in] po_mode PO分割モード
  /// @param[in] bt バックトレーサー
  /// @param[in] dop_list DetectOp のリスト
  /// @param[in] uop_list UntestOp のリスト
  /// @param[in] stats 結果を格納する構造体
  virtual
  void
  run(tDtpgMode mode,
      tDtpgPoMode po_mode,
      BackTracer& bt,
      const vector<DetectOp*>& dop_list,
      const vector<UntestOp*>& uop_list,
      DtpgStats& stats);

  /// @breif 時間計測を制御する．
  virtual
  void
  timer_enable(bool enable);


public:
  //////////////////////////////////////////////////////////////////////
  // 一つの故障に対する処理が終わったときに呼ばれる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンが見つかった場合に呼ばれる関数
  /// @param[in] f 故障
  /// @param[in] tv テストパタン
  void
  set_detected(TpgFault* f,
	       TestVector* tv);

  /// @brief 検出不能のときに呼ばれる関数
  /// @param[in] f 故障
  void
  set_untestable(TpgFault* f);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 一つの故障に対してテストパタン生成を行う．
  /// @param[in] f 故障
  /// @param[in] op テスト生成の結果を処理するファンクター
  void
  dtpg_single(TpgFault* f,
	      BackTracer& bt);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SAT エンジン
  SatEngine* mSatEngine;

  // 対象の回路
  TpgNetwork* mNetwork;

  // mNetwork のノード数
  ymuint32 mMaxId;

  // テストパタンが求められたときに実行するファンクタのリスト
  vector<DetectOp*> mDetectOpList;

  // 検出不能と判定されたときに実行するファンクタのリスト
  vector<UntestOp*> mUntestOpList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @breif 時間計測を制御する．
inline
void
DtpgSat::timer_enable(bool enable)
{
  mSatEngine->timer_enable(enable);
}

END_NAMESPACE_YM_SATPG

#endif // DTPGSAT_H
