﻿#ifndef YMUTILS_MFSET_H
#define YMUTILS_MFSET_H

/// @file YmUtils/MFSet.h
/// @brief MFSet のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "YmTools.h"


BEGIN_NAMESPACE_YM

// クラスの前方参照定義
// ユーザは知る必要はない．
class MFSetCell;


//////////////////////////////////////////////////////////////////////
/// @class MFSet MFSet.h "YmUtils/MFSet.h"
/// @ingroup YmUtils
/// @brief Merge/Find set を実装したクラス
///
/// データ構造とアルゴリズムの教科書でおなじみの Merge/Find-set
/// お互いに素な集合のマージと検索のみを行なう抽象データ型
//////////////////////////////////////////////////////////////////////
class MFSet
{
public:
  //////////////////////////////////////////////////////////////////////
  // 定数
  //////////////////////////////////////////////////////////////////////

  /// @brief 範囲外を表す値
  static
  const ymuint kBadID = static_cast<ymuint>(-1);

public:

  /// @brief コンストラクタ
  /// @param[in] n 確保したい要素の数．
  MFSet(ymuint n);

  /// @brief デストラクタ
  ~MFSet();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素数を返す．
  ymuint
  num() const;

  /// @brief 代表元の検索 (Find)
  /// @param[in] x 要素番号 ( 0 <= x < num() )
  /// @retval 要素 x の属する集合の代表元
  /// @retval kBadID 要素 x が存在していない場合
  ymuint
  find(ymuint x);

  /// @brief 2つの集合の併合 (Merge)
  /// @param[in] x, y 代表元 ( 0 <= x, y < num() )
  /// @retval 新たな代表元を返す．
  /// @retval kBadID x か y が存在していなかった
  /// @note 2つの代表元 x, y の表す集合を併合する．
  /// @note 実は x, y が代表元でない場合，
  /// 内部で find(x), find(y)を呼ぶので処理は行えるが，
  /// 代表元が分かっている場合にはそれを使ったほうが処理は速い．
  ymuint
  merge(ymuint x,
	ymuint y);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 番号 x の要素セルを取ってくる．
  /// そのような要素がない場合にはnullptrを返す．
  MFSetCell*
  get(ymuint x);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 配列の要素数
  ymuint32 mNum;

  // 要素の配列
  MFSetCell* mCellArray;

};

END_NAMESPACE_YM

#endif // YMUTILS_MFSET_H
