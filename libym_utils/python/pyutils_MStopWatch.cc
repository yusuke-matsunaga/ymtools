﻿
/// @file pyutils_MStopWatch.cc
/// @brief MStopWatch の Python 用ラッパ
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2013 Yusuke Matsunaga
/// All rights reserved.


#include "ym_utils/pyutils.h"
#include "ym_utils/USTime.h"
#include "ym_utils/MStopWatch.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// Python 用の構造体定義
//////////////////////////////////////////////////////////////////////

// MStopWatch を表す型
struct MStopWatchObject
{
  // Python のお約束
  PyObject_HEAD

  // MStopWatch の本体
  MStopWatch* mMStopWatch;

};


//////////////////////////////////////////////////////////////////////
// Python 用のメソッド関数定義
//////////////////////////////////////////////////////////////////////

// MStopWatchObject の生成関数
MStopWatchObject*
MStopWatch_new(PyTypeObject* type)
{
  MStopWatchObject* self = PyObject_New(MStopWatchObject, type);
  if ( self == NULL ) {
    return NULL;
  }

  self->mMStopWatch = NULL;

  return self;
}

// MStopWatchObject を開放する関数
void
MStopWatch_dealloc(MStopWatchObject* self)
{
  delete self->mMStopWatch;

  PyObject_Del(self);
}

// 初期化関数
int
MStopWatch_init(MStopWatchObject* self,
		PyObject* args)
{
  ymuint n = 0;
  ymuint id = 0;
  if ( !PyArg_ParseTuple(args, "I|I", &n, &id) ) {
    return NULL;
  }

  if ( self->mMStopWatch != NULL ) {
    delete self->mMStopWatch;
  }
  self->mMStopWatch = new MStopWatch(n, id);

  return 0;
}

// change 関数
PyObject*
MStopWatch_change(MStopWatchObject* self,
		  PyObject* args)
{
  ymuint new_id = 0;
  if ( !PyArg_ParseTuple(args, "I", &new_id) ) {
    return NULL;
  }

  ymuint old_id = self->mMStopWatch->change(new_id);

  return PyObject_FromYmuint32(old_id);
}

// cur_id 関数
PyObject*
MStopWatch_cur_id(MStopWatchObject* self,
		  PyObject* args)
{
  return PyObject_FromYmuint32(self->mMStopWatch->cur_id());
}

// time 関数
PyObject*
MStopWatch_time(MStopWatchObject* self,
		PyObject* args)
{
  ymuint id = 0;
  if ( !PyArg_ParseTuple(args, "I", &id) ) {
    return NULL;
  }

  return PyUSTime_FromUSTime(self->mMStopWatch->time(id));
}


//////////////////////////////////////////////////////////////////////
// MStopWatchObject のメソッドテーブル
//////////////////////////////////////////////////////////////////////
PyMethodDef MStopWatch_methods[] = {
  {"change", (PyCFunction)MStopWatch_change, METH_VARARGS,
   PyDoc_STR("change current timer (uint)")},
  {"cur_id", (PyCFunction)MStopWatch_cur_id, METH_NOARGS,
   PyDoc_STR("return current time ID (NONE)")},
  {"time", (PyCFunction)MStopWatch_time, METH_VARARGS,
   PyDoc_STR("return current time (uint)")},
  {NULL, NULL, 0, NULL}
};

END_NONAMESPACE



//////////////////////////////////////////////////////////////////////
// MStopWatchObject 用のタイプオブジェクト
//////////////////////////////////////////////////////////////////////
PyTypeObject PyMStopWatch_Type = {
  /* The ob_type field must be initialized in the module init function
   * to be portable to Windows without using C++. */
  PyVarObject_HEAD_INIT(NULL, 0)
  "logic.MStopWatch",             // tp_name
  sizeof(MStopWatchObject),       // tp_basicsize
  0,                              // tp_itemsize
  /* methods */
  (destructor)MStopWatch_dealloc, // tp_dealloc
  0,                              // tp_print
  0,                              // tp_getattr
  0,                              // tp_setattr
  0,                              // tp_compare
  0,                              // tp_repr
  0,                              // tp_as_number
  0,                              // tp_as_sequence
  0,                              // tp_as_mapping
  0,                              // tp_hash
  0,                              // tp_call
  0,                              // tp_str
  0,                              // tp_getattro
  0,                              // tp_setattro
  0,                              // tp_as_buffer
  Py_TPFLAGS_DEFAULT,             // tp_flags
  "multi-stopwatch",              // tp_doc
  0,                              // tp_traverse
  0,                              // tp_clear
  0,                              // tp_richcompare
  0,                              // tp_weaklistoffset
  0,                              // tp_iter
  0,                              // tp_iternext
  MStopWatch_methods,             // tp_methods
  0,                              // tp_members
  0,                              // tp_getset
  0,                              // tp_base
  0,                              // tp_dict
  0,                              // tp_descr_get
  0,                              // tp_descr_set
  0,                              // tp_dictoffset
  (initproc)MStopWatch_init,      // tp_init
  0,                              // tp_alloc
  (newfunc)MStopWatch_new,        // tp_new
  0,                              // tp_free
  0,                              // tp_is_gc
};


// @brief PyObject から MStopWatch へのポインタを取り出す．
// @param[in] py_obj Python オブジェクト
// @return MStopWatch へのポインタを返す．
// @note 変換が失敗したら TypeError を送出し，NULL を返す．
MStopWatch*
PyMStopWatch_AsMStopWatchPtr(PyObject* py_obj)
{
  // 型のチェック
  if ( !PyMStopWatch_Check(py_obj) ) {
    PyErr_SetString(PyExc_TypeError, "utils.MStopWatch is expected");
    return NULL;
  }

  // 強制的にキャスト
  MStopWatchObject* my_obj = (MStopWatchObject*)py_obj;

  return my_obj->mMStopWatch;
}

// MStopWatchObject 関係の初期化を行う．
void
MStopWatchObject_init(PyObject* m)
{
  // タイプオブジェクトの初期化
  if ( PyType_Ready(&PyMStopWatch_Type) < 0 ) {
    return;
  }

  // タイプオブジェクトの登録
  PyModule_AddObject(m, "MStopWatch", (PyObject*)&PyMStopWatch_Type);
}

END_NAMESPACE_YM
