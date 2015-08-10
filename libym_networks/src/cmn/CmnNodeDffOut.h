﻿#ifndef CMNNODEDFFOUT_H
#define CMNNODEDFFOUT_H

/// @file CmnNodeDffOut.h
/// @brief CmnNodeDffOut のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "CmnNodeInput.h"


BEGIN_NAMESPACE_YM_NETWORKS_CMN

//////////////////////////////////////////////////////////////////////
/// @class CmnNodeDffOut CmnNodeDffOut.h "CmnNodeDffOut.h"
/// @brief DFF の出力を表す CmnNode の派生クラス
//////////////////////////////////////////////////////////////////////
class CmnNodeDffOut :
  public CmnNodeInput
{
  friend class CmnMgrImpl;

protected:

  /// @brief コンストラクタ
  /// @param[in] dff 親の D-FF
  CmnNodeDffOut(CmnDff* dff);

  /// @brief デストラクタ
  virtual
  ~CmnNodeDffOut();


public:

  /// @brief 関連する D-FF を返す．
  /// @note D-FF に関連していない場合には nullptr を返す．
  virtual
  const CmnDff*
  dff() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 関連するDFF
  const CmnDff* mDff;

};


//////////////////////////////////////////////////////////////////////
/// @calss CmnNodeDffOut1 CmnNodeDffOut.h "CmnNodeDffOut.h"
/// @brief DFF の肯定出力を表す CmnNode の派生クラス
//////////////////////////////////////////////////////////////////////
class CmnNodeDffOut1 :
  public CmnNodeDffOut
{
  friend class CmnMgrImpl;

private:

  /// @brief コンストラクタ
  /// @param[in] dff 親の D-FF
  CmnNodeDffOut1(CmnDff* dff);

  /// @brief デストラクタ
  virtual
  ~CmnNodeDffOut1();


public:

  /// @brief 入力ノードのサブタイプを得る．
  virtual
  tInputType
  input_type() const;

};


//////////////////////////////////////////////////////////////////////
/// @calss CmnNodeDffOut2 CmnNodeDffOut.h "CmnNodeDffOut.h"
/// @brief DFF の肯定出力を表す CmnNode の派生クラス
//////////////////////////////////////////////////////////////////////
class CmnNodeDffOut2 :
  public CmnNodeDffOut
{
  friend class CmnMgrImpl;

private:

  /// @brief コンストラクタ
  /// @param[in] dff 親の D-FF
  CmnNodeDffOut2(CmnDff* dff);

  /// @brief デストラクタ
  virtual
  ~CmnNodeDffOut2();


public:

  /// @brief 入力ノードのサブタイプを得る．
  virtual
  tInputType
  input_type() const;

};

END_NAMESPACE_YM_NETWORKS_CMN

#endif // CMNNODEDFFOUT_H
