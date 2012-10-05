
/// @file ympython.cc
/// @brief ymtools の Python 用の拡張モジュールの実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012 Yusuke Matsunaga
/// All rights reserved.


#include "ympython.h"


BEGIN_NAMESPACE_YM_PYTHON

PyObject* ErrorObject;

// @brief ymint8 から PyObject を生成する．
PyObject*
conv_to_pyobject(ymint8 val)
{
  return Py_BuildValue("b", val);
}

// @brief ymint16 から PyObject を生成する．
PyObject*
conv_to_pyobject(ymint16 val)
{
  return Py_BuildValue("h", val);
}

// @brief ymint32 から PyObject を生成する．
PyObject*
conv_to_pyobject(ymint32 val)
{
  return Py_BuildValue("l", val);
}

// @brief ymint64 から PyObject を生成する．
PyObject*
conv_to_pyobject(ymint64 val)
{
  return Py_BuildValue("L", val);
}

// @brief ymuint8 から PyObject を生成する．
PyObject*
conv_to_pyobject(ymuint8 val)
{
  return Py_BuildValue("B", val);
}

// @brief ymuint16 から PyObject を生成する．
PyObject*
conv_to_pyobject(ymuint16 val)
{
  return Py_BuildValue("H", val);
}

// @brief ymuint32 から PyObject を生成する．
PyObject*
conv_to_pyobject(ymuint32 val)
{
  return Py_BuildValue("k", val);
}

// @brief ymuint64 から PyObject を生成する．
PyObject*
conv_to_pyobject(ymuint64 val)
{
  return Py_BuildValue("K", val);
}

// @brief float から PyObject を生成する．
PyObject*
conv_to_pyobject(float val)
{
  return Py_BuildValue("f", val);
}

// @brief double から PyObject を生成する．
PyObject*
conv_to_pyobject(double val)
{
  return Py_BuildValue("d", val);
}

END_NAMESPACE_YM_PYTHON
