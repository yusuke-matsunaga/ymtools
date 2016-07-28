
/// @file atpg/src/dtpg/DtpgSat.cc
/// @brief DtpgSat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2013 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgSat.h"
#include "TpgNetwork.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "DetectOp.h"
#include "UntestOp.h"


#define VERIFY_MAIMP 0


BEGIN_NAMESPACE_YM_SATPG

Dtpg*
new_DtpgSat()
{
  return new DtpgSat();
}

// @brief コンストラクタ
DtpgSat::DtpgSat()
{
  mSatEngine = new_SatEngine(*this);

  mNetwork = NULL;
}

// @brief デストラクタ
DtpgSat::~DtpgSat()
{
  delete mSatEngine;
}

// @brief 使用する SAT エンジンを指定する．
void
DtpgSat::set_mode(const string& type,
		  const string& option,
		  ostream* outp)
{
  mSatEngine->set_mode(type, option, outp);
}

// @brief 回路と故障リストを設定する．
// @param[in] tgnetwork 対象のネットワーク
void
DtpgSat::set_network(TpgNetwork& tgnetwork)
{
  mNetwork = &tgnetwork;
  mMaxId = mNetwork->node_num();
}

// @brief テスト生成を行なう．
// @param[in] mode メインモード
// @param[in] po_mode PO分割モード
// @param[in] bt バックトレーサー
// @param[in] dop_list DetectOp のリスト
// @param[in] uop_list UntestOp のリスト
// @param[in] stats 結果を格納する構造体
void
DtpgSat::run(tDtpgMode mode,
	     tDtpgPoMode po_mode,
	     BackTracer& bt,
	     const vector<DetectOp*>& dop_list,
	     const vector<UntestOp*>& uop_list,
	     DtpgStats& stats)
{
  mDetectOpList = dop_list;
  mUntestOpList = uop_list;

  mSatEngine->clear_stats();

  mNetwork->activate_all();

  ymuint nn = mNetwork->active_node_num();
  for (ymuint i = 0; i < nn; ++ i) {
    TpgNode* node = mNetwork->active_node(i);

    // 出力の故障
    TpgFault* f0 = node->output_fault(0);
    dtpg_single(f0, bt);

    TpgFault* f1 = node->output_fault(1);
    dtpg_single(f1, bt);

    // 入力の故障
    ymuint ni = node->fanin_num();
    for (ymuint j = 0; j < ni; ++ j) {
      TpgFault* f0 = node->input_fault(0, j);
      dtpg_single(f0, bt);

      TpgFault* f1 = node->input_fault(1, j);
      dtpg_single(f1, bt);
    }
  }

  mSatEngine->get_stats(stats);
}

// @brief テストパタンが見つかった場合に呼ばれる関数
// @param[in] f 故障
// @param[in] tv テストパタン
void
DtpgSat::set_detected(TpgFault* f,
		      TestVector* tv)
{
  for (vector<DetectOp*>::iterator p = mDetectOpList.begin();
       p != mDetectOpList.end(); ++ p) {
    DetectOp& op = **p;
    op(f, tv);
  }
}

// @brief 検出不能のときに呼ばれる関数
// @param[in] f 故障
void
DtpgSat::set_untestable(TpgFault* f)
{
  for (vector<UntestOp*>::iterator p = mUntestOpList.begin();
       p != mUntestOpList.end(); ++ p) {
    UntestOp& op = **p;
    op(f);
  }
}

// @brief 一つの故障に対してテストパタン生成を行う．
// @param[in] f 故障
void
DtpgSat::dtpg_single(TpgFault* fault,
		     BackTracer& bt)
{
  if ( fault != NULL &&
       fault->is_rep() &&
       fault->status() != kFsDetected &&
       !fault->is_skip() ) {
    mSatEngine->run(fault, mMaxId, bt);
  }
}

END_NAMESPACE_YM_SATPG
