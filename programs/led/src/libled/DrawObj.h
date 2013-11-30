#ifndef DRAWOBJ_H
#define DRAWOBJ_H

/// @file DrawObj.h
/// @brief DrawObj のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#include "led_nsdef.h"


BEGIN_NAMESPACE_YM_LED

//////////////////////////////////////////////////////////////////////
/// @class DrawObj DrawObj.h "DrawObj.h"
/// @brief 描画用のパーツを表すクラス
//////////////////////////////////////////////////////////////////////
class DrawObj
{
public:

  /// @brief デストラクタ
  virtual
  ~DrawObj() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが用意する仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 描画する．
  virtual
  void
  draw(QPainter& painter) = 0;

  /// @brief 描画領域を囲む最小の矩形を求める．
  virtual
  QRectF
  bounding_box() = 0;

};


//////////////////////////////////////////////////////////////////////
/// @class LineObj DrawObj.h "DrawObj.h"
/// @brief 線分を描画するオブジェクト
//////////////////////////////////////////////////////////////////////
class LineObj :
  public DrawObj
{
public:

  /// @brief コンストラクタ
  /// @param[in] point1, point2 線分の両端点
  LineObj(const QPointF& point1,
	  const QPointF& point2);

  /// @brief デストラクタ
  virtual
  ~LineObj();


public:
  //////////////////////////////////////////////////////////////////////
  // DrawObj の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 描画する．
  virtual
  void
  draw(QPainter& painter);

  /// @brief 描画領域を囲む最小の矩形を求める．
  virtual
  QRectF
  bounding_box();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 描画用の点
  QPointF mPoints[2];

};


//////////////////////////////////////////////////////////////////////
/// @class PointsObj DrawObj.h "DrawObj.h"
/// @brief 点列をデータとするクラスの親クラス
//////////////////////////////////////////////////////////////////////
class PointsObj :
  public DrawObj
{
public:

  /// @brief コンストラクタ
  /// @param[in] points 設定する点列
  PointsObj(const vector<QPointF>& points);

  /// @brief デストラクタ
  virtual
  ~PointsObj();


public:
  //////////////////////////////////////////////////////////////////////
  // DrawObj の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 描画領域を囲む最小の矩形を求める．
  virtual
  QRectF
  bounding_box();


protected:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 点列の要素数
  ymuint32 mPointNum;

  // 描画用の点列
  QPointF* mPoints;

};


//////////////////////////////////////////////////////////////////////
/// @class PolylineObj DrawObj.h "DrawObj.h"
/// @brief 折れ線を描画するオブジェクト
//////////////////////////////////////////////////////////////////////
class PolylineObj :
  public PointsObj
{
public:

  /// @brief コンストラクタ
  /// @param[in] points 設定する点列
  PolylineObj(const vector<QPointF>& points);

  /// @brief デストラクタ
  virtual
  ~PolylineObj();


public:
  //////////////////////////////////////////////////////////////////////
  // DrawObj の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 描画する．
  virtual
  void
  draw(QPainter& painter);

};


//////////////////////////////////////////////////////////////////////
/// @class PolygonObj DrawObj.h "DrawObj.h"
/// @brief ポリゴンを描画するオブジェクト
//////////////////////////////////////////////////////////////////////
class PolygonObj :
  public PointsObj
{
public:

  /// @brief コンストラクタ
  /// @param[in] points 設定する点列
  PolygonObj(const vector<QPointF>& points);

  /// @brief デストラクタ
  virtual
  ~PolygonObj();


public:
  //////////////////////////////////////////////////////////////////////
  // DrawObj の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 描画する．
  virtual
  void
  draw(QPainter& painter);

};


//////////////////////////////////////////////////////////////////////
/// @class EllipseObj DrawObj.h "DrawObj.h"
/// @brief 楕円を描画するオブジェクト
//////////////////////////////////////////////////////////////////////
class EllipseObj :
  public DrawObj
{
public:

  /// @brief コンストラクタ
  /// @param[in] rect 楕円を囲む矩形
  EllipseObj(const QRectF& rect);

  /// @brief デストラクタ
  virtual
  ~EllipseObj();


public:
  //////////////////////////////////////////////////////////////////////
  // DrawObj の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 描画する．
  virtual
  void
  draw(QPainter& painter);

  /// @brief 描画領域を囲む最小の矩形を求める．
  virtual
  QRectF
  bounding_box();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 楕円を囲む矩形
  QRectF mRect;

};

END_NAMESPACE_YM_LED

#endif // DRAWOBJ_H
