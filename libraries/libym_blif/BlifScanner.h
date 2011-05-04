#ifndef LIBYM_BLIF_BLIFSCANNER_H
#define LIBYM_BLIF_BLIFSCANNER_H

/// @file libym_blif/BlifScanner.h
/// @brief BlibScanner のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// $Id: BlifScanner.h 1978 2009-02-06 12:29:16Z matsunaga $
///
/// Copyright (C) 2005-2010 Yusuke Matsunaga
/// All rights reserved.


#include "ym_blif/blif_nsdef.h"
#include "ym_utils/FileScanner.h"
#include "ym_utils/StrBuff.h"
#include "BlifDic.h"


BEGIN_NAMESPACE_YM_BLIF

//////////////////////////////////////////////////////////////////////
/// @class BlifScanner BlifScanner.h "BlifScanner.h"
/// @brief blif 用の字句解析器
//////////////////////////////////////////////////////////////////////
class BlifScanner :
  public FileScanner
{
public:

  /// @brief コンストラクタ
  BlifScanner();

  /// @brief デストラクタ
  ~BlifScanner();


public:

  /// @brief トークンを一つ読み出す．
  /// @param[out] loc トークンの位置を格納する変数
  tToken
  read_token(FileRegion& loc);

  /// @brief 最後の get_token() で読み出した字句の文字列を返す．
  const char*
  cur_string();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief read_token() の下請け関数
  /// @return トークンを返す．
  tToken
  scan();

  /// @brief 予約後の検査をする．
  /// @param[in] start_with_dot '.' で始まっている時に true を渡す．
  /// @return トークンを返す．
  tToken
  check_word(bool start_with_dot);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 予約語テーブル
  BlifDic mDic;

  // 文字列バッファ
  StrBuff mCurString;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 最後の get_token() で読み出した字句の文字列を返す．
inline
const char*
BlifScanner::cur_string()
{
  return mCurString.c_str();
}

END_NAMESPACE_YM_BLIF

#endif // LIBYM_BLIF_BLIFSCANNER_H
