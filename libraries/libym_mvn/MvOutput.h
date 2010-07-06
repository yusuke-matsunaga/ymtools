#ifndef LIBYM_MVN_MVOUTPUT_H
#define LIBYM_MVN_MVOUTPUT_H

/// @file libym_mvn/MvOutput.h
/// @brief MvOutput のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010 Yusuke Matsunaga
/// All rights reserved.


#include "ym_mvn/MvNode.h"
#include "ym_mvn/MvPin.h"


BEGIN_NAMESPACE_YM_MVN

//////////////////////////////////////////////////////////////////////
/// @class MvOutput MvOutput.h "MvOutput.h"
/// @brief 出力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class MvOutput :
  public MvNode
{
  friend class MvMgr;

public:

  /// @brief ノードの種類を得る．
  virtual
  tType
  type() const;

  /// @brief 入力ピン数を得る．
  virtual
  ymuint
  input_num() const;

  /// @brief 出力ピン数を得る．
  virtual
  ymuint
  output_num() const;

  /// @brief 入力ノード/出力ノードの場合に位置を返す．
  /// @note type() が kOutput と kOutput の時のみ意味を持つ．
  virtual
  ymuint
  pos() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力ピンを得る．
  /// @param[in] pos 位置 ( 0 <= pos < input_num() )
  virtual
  MvInputPin*
  input(ymuint pos);

  /// @brief 出力ピンを得る．
  /// @param[in] pos 位置 ( 0 <= pos < output_num() )
  virtual
  MvOutputPin*
  output(ymuint pos);


private:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ / デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] module 親のモジュール
  /// @param[in] pos 位置
  /// @param[in] bit_width ビット幅
  MvOutput(MvModule* module,
	   ymuint pos,
	   ymuint bit_width);

  /// @brief デストラクタ
  ~MvOutput();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力ピン
  MvInputPin mPin;
  
  // 位置
  ymuint32 mPos;

};

END_NAMESPACE_YM_MVN

#endif // LIBYM_MVN_MVOUTPUT_H