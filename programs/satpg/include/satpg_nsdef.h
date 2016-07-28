#ifndef SATPG_NSDEF_H
#define SATPG_NSDEF_H

/// @file satpg_nsdef.h
/// @brief ATPG 用の名前空間の定義
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012 Yusuke Matsunaga
/// All rights reserved.

#include "ymtools.h"


// 名前空間の定義ファイル
// 必ず先頭でインクルードしなければならない．

//////////////////////////////////////////////////////////////////////
// 名前空間の定義用マクロ
// namespace でネストするのがいやなので define マクロでごまかす．
//////////////////////////////////////////////////////////////////////

#define BEGIN_NAMESPACE_YM_SATPG \
BEGIN_NAMESPACE_YM \
BEGIN_NAMESPACE(nsSatpg)

#define END_NAMESPACE_YM_SATPG \
END_NAMESPACE(nsSatpg) \
END_NAMESPACE_YM


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @brief タイマーのモード
//////////////////////////////////////////////////////////////////////
enum {
  /// @brief その他
  TM_MISC = 0,
  /// @brief ファイル読み込み
  TM_READ = 1,
  /// @brief DTPG
  TM_DTPG = 2,
  /// @brief 故障シミュレーション
  TM_FSIM = 3,
  /// @brief SAT
  TM_SAT  = 4,
  /// @brief タイマーのモード数
  TM_SIZE = 5
};


//////////////////////////////////////////////////////////////////////
/// @brief シミュレーションモード
//////////////////////////////////////////////////////////////////////
enum tSimMode {
  /// @brief なし
  kSimNone,
  /// @brief シングルパタン
  kSimSingle,
  /// @brief パタン並列
  kSimPpsfp
};


//////////////////////////////////////////////////////////////////////
/// @brief DTPG モード
//////////////////////////////////////////////////////////////////////
enum tDtpgMode {
  /// @brief single モード
  kDtpgSingle,
  /// @brief dual モード
  kDtpgDual,
  /// @brief node モード
  kDtpgNode,
  /// @brief ffr モード
  kDtpgFFR,
  /// @brief mffc モード
  kDtpgMFFC,
  /// @brief all モード
  kDtpgAll
};


//////////////////////////////////////////////////////////////////////
/// @brief PO モード
//////////////////////////////////////////////////////////////////////
enum tDtpgPoMode {
  /// @brief 分割なし
  kDtpgPoNone,
  /// @brief 分割あり(サイズの昇順)
  kDtpgPoInc,
  /// @brief 分割あり(サイズの降順)
  kDtpgPoDec
};


//////////////////////////////////////////////////////////////////////
// クラス名の宣言
//////////////////////////////////////////////////////////////////////

class AtpgMgr;
class TpgFault;
class FaultMgr;
class TpgNetwork;
class TpgNode;
class TpgPrimitive;
class BackTracer;
class DetectOp;
class UntestOp;
class TestVector;
class TvMgr;
class Dtpg;
struct DtpgStats;
class Rtpg;
class RtpgStats;
class MinPat;
class MinPatStats;
class Fsim;
class FsimOld;
class FsimOp1;
class FsimOp2;

END_NAMESPACE_YM_SATPG

#endif // SATPG_NSDEF_H
