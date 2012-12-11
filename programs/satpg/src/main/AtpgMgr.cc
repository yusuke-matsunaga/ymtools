
/// @file src/main/AtpgMgr.cc
/// @brief AtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: AtpgMgr.cc 2203 2009-04-16 05:04:40Z matsunaga $
///
/// Copyright (C) 2005-2010, 2012 Yusuke Matsunaga
/// All rights reserved.


#include "AtpgMgr.h"
#include "fsim/Fsim.h"
#include "dtpg/DtpgSat.h"
#include "ym_networks/TgBlifReader.h"
#include "ym_networks/TgIscas89Reader.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス FsimNetBinder
//////////////////////////////////////////////////////////////////////
class FsimNetBinder :
  public T2Binder<const TgNetwork&, const vector<SaFault*>&>
{
public:

  /// @brief コンストラクタ
  FsimNetBinder(Fsim* fsim);

  /// @brief イベント処理関数
  virtual
  void
  event_proc(const TgNetwork& network,
	     const vector<SaFault*>& fault_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // Fsim
  Fsim* mFsim;

};


// @brief コンストラクタ
FsimNetBinder::FsimNetBinder(Fsim* fsim) :
  mFsim(fsim)
{
}

// @brief イベント処理関数
void
FsimNetBinder::event_proc(const TgNetwork& network,
			  const vector<SaFault*>& fault_list)
{
  mFsim->set_network(network, fault_list);
}


//////////////////////////////////////////////////////////////////////
// AtpgMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
AtpgMgr::AtpgMgr() :
  mTimer(TM_SIZE, TM_MISC)
{
  mFsim = new_Fsim();

  mDtpg = new DtpgSat();

  reg_network_handler(new FsimNetBinder(mFsim));

  set_dtpg_mode();
}

// @brief デストラクタ
AtpgMgr::~AtpgMgr()
{
  delete mFsim;
  delete mDtpg;
}

// @brief blif 形式のファイルを読み込む．
// @param[in] filename ファイル名
// @param[in] cell_library セルライブラリ
// @retval true 読み込みが成功した．
// @retval false 読み込みが失敗した．
bool
AtpgMgr::read_blif(const string& filename,
		   const CellLibrary* cell_library)
{
  ymuint old_id = mTimer.cur_id();
  mTimer.change(TM_READ);

  TgBlifReader read;
  bool stat = read(filename, mNetwork, cell_library);
  if ( stat ) {
    after_set_network();
  }

  mTimer.change(old_id);

  return stat;
}

// @brief iscas89 形式のファイルを読み込む．
// @param[in] filename ファイル名
// @retval true 読み込みが成功した．
// @retval false 読み込みが失敗した．
bool
AtpgMgr::read_iscas89(const string& filename)
{
  ymuint old_id = mTimer.cur_id();
  mTimer.change(TM_READ);

  TgIscas89Reader read;
  bool stat = read(filename, mNetwork);
  if ( stat ) {
    after_set_network();
  }

  mTimer.change(old_id);

  return stat;
}

// @brief 一つのテストベクタに対する故障シミュレーションを行なう．
// @param[in] tv テストベクタ
// @param[out] det_faults 検出された故障を格納するリスト
void
AtpgMgr::fsim(TestVector* tv,
	      vector<SaFault*>& det_faults)
{
  ymuint old_id = mTimer.cur_id();
  mTimer.change(TM_FSIM);

  mFsim->run(tv, det_faults);

  ymuint det_count = det_faults.size();
  if ( det_count > 0 ) {
    for (vector<SaFault*>::const_iterator p = det_faults.begin();
	 p != det_faults.end(); ++ p) {
      SaFault* f = *p;
      if ( f->status() == kFsUndetected ) {
	mFaultMgr.set_status(f, kFsDetected);
      }
    }
    mFaultMgr.update();
    after_update_faults();
  }

  mTimer.change(old_id);
}

// @brief 複数のテストベクタに対する故障シミュレーションを行なう．
// @param[in] tv_list テストベクタのリスト
// @param[out] det_faults 検出された故障を格納するリストのリスト
void
AtpgMgr::fsim(const vector<TestVector*>& tv_list,
	      vector<vector<SaFault*> >& det_faults_list)
{
  ymuint old_id = mTimer.cur_id();
  mTimer.change(TM_FSIM);

  mFsim->run(tv_list, det_faults_list);

  ymuint det_count = 0;
  for (ymuint i = 0; i < tv_list.size(); ++ i) {
    const vector<SaFault*>& det_faults = det_faults_list[i];
    ymuint det_count1 = det_faults.size();
    if ( det_count1 > 0 ) {
      det_count += det_count1;
      for (vector<SaFault*>::const_iterator p = det_faults.begin();
	   p != det_faults.end(); ++ p) {
	SaFault* f = *p;
	if ( f->status() == kFsUndetected ) {
	  mFaultMgr.set_status(f, kFsDetected);
	}
      }
    }
  }

  if ( det_count > 0 ) {
    mFaultMgr.update();
    after_update_faults();
  }

  mTimer.change(old_id);
}

// @brief 一つのパタンで一つの故障に対するシミュレーションを行う．
// @param[in] tv テストベクタ
// @param[in] f 対象の故障
// @retval true tv で f の検出ができた．
// @retval false tv では f の検出ができない．
bool
AtpgMgr::fsim(TestVector* tv,
	      SaFault* f)
{
  ymuint old_id = mTimer.cur_id();
  mTimer.change(TM_FSIM);

  bool stat = mFsim->run(tv, f);
  if ( stat ) {
    if ( f->status() == kFsUndetected ) {
      mFaultMgr.set_status(f, kFsDetected);
      mFaultMgr.update();
      after_update_faults();
    }
  }

  mTimer.change(old_id);

  return stat;
}

// @brief 使用する SAT エンジンを指定する．
void
AtpgMgr::set_dtpg_mode(const string& type,
		       const string& option,
		       ostream* outp)
{
  mDtpg->set_mode(type, option, outp);
}

// @brief 一つの故障に対してテストパタン生成を行なう．
void
AtpgMgr::dtpg_single()
{

  ymuint old_id = mTimer.cur_id();
  mTimer.change(TM_DTPG);

  vector<SaFault*> d_list;
  vector<SaFault*> u_list;
  mDtpg->dtpg_single(mTvMgr, d_list, u_list);

  update_faults(d_list, u_list);

  mTimer.change(old_id);
}

// @brief 同じ位置の2つの故障に対してテストパタン生成を行なう．
void
AtpgMgr::dtpg_dual()
{
  ymuint old_id = mTimer.cur_id();
  mTimer.change(TM_DTPG);

  vector<SaFault*> d_list;
  vector<SaFault*> u_list;
  mDtpg->dtpg_dual(mTvMgr, d_list, u_list);

  update_faults(d_list, u_list);

  mTimer.change(old_id);
}

// @brief FFR 内の故障に対してテストパタン生成を行なう．
void
AtpgMgr::dtpg_ffr()
{
  ymuint old_id = mTimer.cur_id();
  mTimer.change(TM_DTPG);

  vector<SaFault*> d_list;
  vector<SaFault*> u_list;
  mDtpg->dtpg_ffr(mTvMgr, d_list, u_list);

  update_faults(d_list, u_list);

  mTimer.change(old_id);
}

// @brief MFFC 内の故障に対してテストパタン生成を行なう．
void
AtpgMgr::dtpg_mffc()
{
  ymuint old_id = mTimer.cur_id();
  mTimer.change(TM_DTPG);

  vector<SaFault*> d_list;
  vector<SaFault*> u_list;
  mDtpg->dtpg_mffc(mTvMgr, d_list, u_list);

  update_faults(d_list, u_list);

  mTimer.change(old_id);
}

// @brief 全ての故障に対してテストパタン生成を行なう．
void
AtpgMgr::dtpg_all()
{
  ymuint old_id = mTimer.cur_id();
  mTimer.change(TM_DTPG);

  vector<SaFault*> d_list;
  vector<SaFault*> u_list;
  mDtpg->dtpg_all(mTvMgr, d_list, u_list);

  update_faults(d_list, u_list);

  mTimer.change(old_id);
}

// @brief 一つの故障に対してテストパタン生成を行なう．
void
AtpgMgr::dtpg_single_posplit()
{
  ymuint old_id = mTimer.cur_id();
  mTimer.change(TM_DTPG);

  ymuint no = _network().output_num2();
  for (ymuint po_pos = 0; po_pos < no; ++ po_pos) {
    vector<SaFault*> d_list;
    vector<SaFault*> u_list;
    mDtpg->dtpg_single_posplit(po_pos, mTvMgr, d_list, u_list);
    update_faults(d_list, vector<SaFault*>());
  }
  vector<SaFault*> d_list;
  vector<SaFault*> u_list;
  mDtpg->dtpg_single(mTvMgr, d_list, u_list);
  update_faults(d_list, u_list);

  mTimer.change(old_id);
}

// @brief 同じ位置の2つの故障に対してテストパタン生成を行なう．
void
AtpgMgr::dtpg_dual_posplit()
{
  ymuint old_id = mTimer.cur_id();
  mTimer.change(TM_DTPG);

  ymuint no = _network().output_num2();
  for (ymuint po_pos = 0; po_pos < no; ++ po_pos) {
    vector<SaFault*> d_list;
    vector<SaFault*> u_list;
    mDtpg->dtpg_dual_posplit(po_pos, mTvMgr, d_list, u_list);

    update_faults(d_list, vector<SaFault*>());
  }
  vector<SaFault*> d_list;
  vector<SaFault*> u_list;
  mDtpg->dtpg_dual(mTvMgr, d_list, u_list);

  update_faults(d_list, u_list);

  mTimer.change(old_id);
}

// @brief FFR 内の故障に対してテストパタン生成を行なう．
void
AtpgMgr::dtpg_ffr_posplit()
{
  ymuint old_id = mTimer.cur_id();
  mTimer.change(TM_DTPG);

  ymuint no = _network().output_num2();
  for (ymuint po_pos = 0; po_pos < no; ++ po_pos) {
    vector<SaFault*> d_list;
    vector<SaFault*> u_list;
    mDtpg->dtpg_ffr_posplit(po_pos, mTvMgr, d_list, u_list);

    update_faults(d_list, vector<SaFault*>());
  }
  vector<SaFault*> d_list;
  vector<SaFault*> u_list;
  mDtpg->dtpg_ffr(mTvMgr, d_list, u_list);

  update_faults(d_list, u_list);

  mTimer.change(old_id);
}

// @brief MFFC 内の故障に対してテストパタン生成を行なう．
void
AtpgMgr::dtpg_mffc_posplit()
{
  ymuint old_id = mTimer.cur_id();
  mTimer.change(TM_DTPG);

  ymuint no = _network().output_num2();
  for (ymuint po_pos = 0; po_pos < no; ++ po_pos) {
    vector<SaFault*> d_list;
    vector<SaFault*> u_list;
    mDtpg->dtpg_mffc_posplit(po_pos, mTvMgr, d_list, u_list);

    update_faults(d_list, vector<SaFault*>());
  }
  vector<SaFault*> d_list;
  vector<SaFault*> u_list;
  mDtpg->dtpg_mffc(mTvMgr, d_list, u_list);

  update_faults(d_list, u_list);

  mTimer.change(old_id);
}

// @brief 全ての故障に対してテストパタン生成を行なう．
void
AtpgMgr::dtpg_all_posplit()
{
  ymuint old_id = mTimer.cur_id();
  mTimer.change(TM_DTPG);

  ymuint no = _network().output_num2();
  for (ymuint po_pos = 0; po_pos < no; ++ po_pos) {
    vector<SaFault*> d_list;
    vector<SaFault*> u_list;
    mDtpg->dtpg_all_posplit(po_pos, mTvMgr, d_list, u_list);

    update_faults(d_list, vector<SaFault*>());
  }
  vector<SaFault*> d_list;
  vector<SaFault*> u_list;
  mDtpg->dtpg_all(mTvMgr, d_list, u_list);

  update_faults(d_list, u_list);

  mTimer.change(old_id);
}

// @brief ファイル読み込みに関わる時間を得る．
USTime
AtpgMgr::read_time() const
{
  return mTimer.time(TM_READ);
}

// @brief DTPG に関わる時間を得る．
USTime
AtpgMgr::dtpg_time() const
{
  return mTimer.time(TM_DTPG);
}

// @brief 故障シミュレーションに関わる時間を得る．
USTime
AtpgMgr::fsim_time() const
{
  return mTimer.time(TM_FSIM);
}

// @brief SAT に関わる時間を得る．
USTime
AtpgMgr::sat_time() const
{
  return mTimer.time(TM_SAT);
}

// @brief その他の時間を得る．
USTime
AtpgMgr::misc_time() const
{
  return mTimer.time(TM_MISC);
}

// @brief 故障リストの更新を行なう．
// @param[in] d_list 検出された故障のリスト
// @param[in] u_list 検出不能と判定された故障のリスト
void
AtpgMgr::update_faults(const vector<SaFault*>& d_list,
		       const vector<SaFault*>& u_list)
{
  for (vector<SaFault*>::const_iterator p = d_list.begin();
       p != d_list.end(); ++ p) {
    SaFault* f = *p;
    mFaultMgr.set_status(f, kFsDetected);
  }
  for (vector<SaFault*>::const_iterator p = u_list.begin();
       p != u_list.end(); ++ p) {
    SaFault* f = *p;
    mFaultMgr.set_status(f, kFsUntestable);
  }

  mFaultMgr.update();
  after_update_faults();
}

// @brief ネットワークをセットした後に呼ぶ関数
void
AtpgMgr::after_set_network()
{
  mFaultMgr.clear();
  mFaultMgr.set_ssa_fault(mNetwork);

  mTvMgr.clear();
  mTvMgr.init(mNetwork.input_num2());

  mDtpg->set_network(mNetwork, mFaultMgr.remain_list());

  mNtwkBindMgr.prop_event(mNetwork, mFaultMgr.remain_list());
}

// @brief 故障リストが変更された時に呼ばれる関数
void
AtpgMgr::after_update_faults()
{
  mFaultBindMgr.prop_event(mFaultMgr.remain_list());
}

END_NAMESPACE_YM_SATPG