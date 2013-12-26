#ifndef COMPRESSDECODER_H
#define COMPRESSDECODER_H

/// @file CompressDecoder.h
/// @brief CompressDecoder のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#include "ym_utils/FileDecoder.h"
#include "FileBuff.h"
#include "compress_common.h"


BEGIN_NAMESPACE_YM_COMPCOMMON

//////////////////////////////////////////////////////////////////////
/// @class CompressDecoder CompressDecoder.h "CompressDecoder.h"
/// @brief compress(Z) 形式の伸長器
//////////////////////////////////////////////////////////////////////
class CompressDecoder :
  public FileDecoder
{
public:

  /// @brief コンストラクタ
  CompressDecoder();

  /// @brief デストラクタ
  virtual
  ~CompressDecoder();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファイルを開く
  /// @param[in] filename ファイル名
  /// @retval true 成功した
  /// @retval false 失敗した．
  ///
  /// 失敗する理由は以下の通り
  ///  - ファイルが存在しない．
  ///  - ファイルに対する読み出し許可がない．
  ///  - ファイルの形式が異なる．
  virtual
  bool
  open(const char* filename);

  /// @brief ファイルを閉じる．
  virtual
  void
  close();

  /// @brief 読み出せるデータがある時に true を返す．
  virtual
  bool
  is_ready() const;

  /// @brief 圧縮されたファイルを読んで最大 num バイトをバッファに格納する．
  /// @param[in] rbuff 展開したデータを格納するバッファ
  /// @param[in] num 読み出すデータ数(バイト)
  /// @return 実際に読み出したバイト数を返す．
  /// @note エラーが起こったら -1 を返す．
  virtual
  ssize_t
  read(ymuint8* rbuff,
       ymuint64 num);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  code_int
  getcode();

  /// @brief スタックを初期化する．
  void
  init_stack();

  /// @brief スタックが空の時 true を返す．
  bool
  is_empty();

  /// @brief スタックにデータを積む．
  void
  push_stack(char_type data);

  /// @brief スタックからデータを取り出す．
  char_type
  pop_stack();

  u_short&
  tab_prefixof(ymuint i);

  char_type&
  tab_suffixof(ymuint i);

  /// @brief num バイトを読み込み buff[] に格納する．
  /// @param[in] buff データを格納するバッファ
  /// @param[in] num 読み込むバイト数．
  /// @return 実際に読み込んだバイト数を返す．
  ssize_t
  _read(ymuint8* buff,
	ymuint64 num);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // コードあたりのビット数
  ymuint32 m_n_bits;

  // n_bits の最大値
  ymuint32 m_maxbits;

  // Maximum code
  code_int m_maxcode;

  // Should NEVER generate this code
  code_int m_maxmaxcode;

  tState m_state;

  code_int m_free_ent;

  ymint32 m_block_compress;

  ymint32 m_clear_flg;

  char_type mStack[8000];

  char_type* mStackPtr;

  u_short mPrefix[k_HSIZE];

  char_type mSuffix[1 << k_BITS];

  ymint32 m_finchar;

  code_int m_oldcode;

  code_int m_incode;

  ymint32 m_roffset;

  ymint32 m_size;

  char_type m_gbuf[k_BITS];

  // ファイルバッファ
  FileBuff mBuff;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

inline
u_short&
CompressDecoder::tab_prefixof(ymuint i)
{
  return mPrefix[i];
}

inline
char_type&
CompressDecoder::tab_suffixof(ymuint i)
{
  return mSuffix[i];
}

// @brief スタックを初期化する．
inline
void
CompressDecoder::init_stack()
{
  mStackPtr = &mStack[0];
}

// @brief スタックが空の時 true を返す．
inline
bool
CompressDecoder::is_empty()
{
  return mStackPtr == &mStack[0];
}

// @brief スタックにデータを積む．
inline
void
CompressDecoder::push_stack(char_type data)
{
  assert_cond( mStackPtr < &mStack[8000], __FILE__, __LINE__);
  *(mStackPtr ++) = data;
}

// @brief スタックからデータを取り出す．
inline
char_type
CompressDecoder::pop_stack()
{
  return *(-- mStackPtr);
}

END_NAMESPACE_YM_COMPCOMMON

BEGIN_NAMESPACE_YM

using nsCompCommon::CompressDecoder;

END_NAMESPACE_YM

#endif // COMPRESSDECODER_H
