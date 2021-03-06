﻿
/// @file pycell_CellClass.cc
/// @brief CellClass の Python 用ラッパ
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012 Yusuke Matsunaga
/// All rights reserved.


#include "YmCell/pycell.h"
#include "YmCell/CellClass.h"
#include "YmCell/CellGroup.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// Python 用の構造体定義
//////////////////////////////////////////////////////////////////////

// CellClass を表す型
struct CellClassObject
{
  // Python のお約束
  PyObject_HEAD

  // CellClass のポインタ
  const CellClass* mClass;

  // 同位体変換のリスト
  PyObject* mIdMapList;

  // セルグループのリスト
  PyObject* mCellGroupList;

};


//////////////////////////////////////////////////////////////////////
// Python 用のメソッド関数定義
//////////////////////////////////////////////////////////////////////

// CellClassObject を開放する関数
void
CellClass_dealloc(CellClassObject* self)
{
  Py_DECREF(self->mIdMapList);
  Py_DECREF(self->mCellGroupList);

  PyObject_Del(self);
}

// id 関数
PyObject*
CellClass_id(CellClassObject* self,
	     PyObject* args)
{
  return PyObject_FromYmuint32(self->mClass->id());
}

// idmap_list 関数
PyObject*
CellClass_idmap_list(CellClassObject* self,
		     PyObject* args)
{
  PyObject* result = self->mIdMapList;

  Py_INCREF(result);
  return result;
}

// group_list 関数
PyObject*
CellClass_group_list(CellClassObject* self,
		     PyObject* args)
{
  PyObject* result = self->mCellGroupList;

  Py_INCREF(result);
  return result;
}


//////////////////////////////////////////////////////////////////////
// CellClassObject のメソッドテーブル
//////////////////////////////////////////////////////////////////////
PyMethodDef CellClass_methods[] = {
  // PyMethodDef のフィールド
  //   char*       ml_name;
  //   PyCFunction ml_meth;
  //   int         ml_flags;
  //   char*       ml_doc;
  // ml_flags で使用可能なマクロは以下の通り
  //  - METH_VARARGS
  //  - METH_KEYWORDS
  //  - METH_NOARGS
  //  - METH_O
  //  - METH_OLDARGS (obsolete)
  //  - METH_CLASS
  //  - METH_STATIC
  //  - METH_COEXIST
  {"id", (PyCFunction)CellClass_id, METH_NOARGS,
   PyDoc_STR("return ID")},
  {"idmap_list", (PyCFunction)CellClass_idmap_list, METH_NOARGS,
   PyDoc_STR("return ident map list")},
  {"cell_group_list", (PyCFunction)CellClass_group_list, METH_NOARGS,
   PyDoc_STR("return group list")},

  {NULL, NULL, 0, NULL} // end-marker
};

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// CellClassObject 用のタイプオブジェクト
//////////////////////////////////////////////////////////////////////
PyTypeObject PyCellClass_Type = {
  /* The ob_type field must be initialized in the module init function
   * to be portable to Windows without using C++. */
  PyVarObject_HEAD_INIT(NULL, 0)
  "cell_lib.CellClass",             // tp_name
  sizeof(CellClassObject),          // tp_basicsize
  (int)0,                           // tp_itemsize

  // Methods to implement standard operations

  (destructor)CellClass_dealloc,    // tp_dealloc
  (printfunc)0,                     // tp_print
  (getattrfunc)0,                   // tp_getattr
  (setattrfunc)0,                   // tp_setattr
  (cmpfunc)0,                       // tp_compare
  (reprfunc)0,                      // tp_repr

  // Method suites for standard classes
  0,                                // tp_as_number
  0,                                // tp_as_sequence
  0,                                // tp_as_mapping

  // More standard operations (here for binary compatibility)
  (hashfunc)0,                      // tp_hash
  (ternaryfunc)0,                   // tp_call
  (reprfunc)0,                      // tp_str
  (getattrofunc)0,                  // tp_getattro
  (setattrofunc)0,                  // tp_setattro

  // Functions to access object as input/output buffer
  0,                                // tp_as_buffer

  // Flags to define presence of optional/expanded features
  Py_TPFLAGS_DEFAULT,               // tp_flags

  // Documentation string
  "Cell Class",                     // tp_doc

  // Assigned meaning in release 2.0

  // call function for all accesible objects
  (traverseproc)0,                  // tp_traverse

  // delete references to contained objects
  (inquiry)0,                       // tp_clear

  // Assigned meaning in release 2.1

  // rich comparisons
  (richcmpfunc)0,                   // tp_richcompare

  // weak reference enabler
  (long)0,                          // tp_weaklistoffset

  // Added in release 2.2

  // Iterators
  (getiterfunc)0,                   // tp_iter
  (iternextfunc)0,                  // tp_iternext

  // Attribute descriptor and subclassing stuff
  CellClass_methods,                // tp_methods
  0,                                // tp_members
  0,                                // tp_getset
  (struct _typeobject*)0,           // tp_base
  (PyObject*)0,                     // tp_dict
  (descrgetfunc)0,                  // tp_descr_get
  (descrsetfunc)0,                  // tp_descr_set
  (long)0,                          // tp_dictoffset
  (initproc)0,                      // tp_init
  (allocfunc)0,                     // tp_alloc
  (newfunc)0,                       // tp_new
  (freefunc)0,                      // tp_free
  (inquiry)0,                       // tp_is_gc

  (PyObject*)0,                     // tp_bases
  (PyObject*)0,                     // tp_mro (method resolution order)
  (PyObject*)0,                     // tp_cache
  (PyObject*)0,                     // tp_subclasses
  (PyObject*)0                      // tp_weaklist
};


//////////////////////////////////////////////////////////////////////
// PyObject と CellClass の間の変換関数
//////////////////////////////////////////////////////////////////////

// @brief CellClass から CellClassObject を生成する．
// @param[in] cell_class セルクラス
PyObject*
PyCellClass_FromCellClass(const CellClass* cell_class,
			  PyObject* group_list[])
{
  CellClassObject* self = PyObject_New(CellClassObject, &PyCellClass_Type);
  if ( self == NULL ) {
    return NULL;
  }

  self->mClass = cell_class;

  ymuint n1 = cell_class->idmap_num();
  self->mIdMapList = PyList_New(n1);
  for (ymuint i = 0; i < n1; ++ i) {
    // 未完
  }

  ymuint n2 = cell_class->group_num();
  self->mCellGroupList = PyList_New(n2);
  for (ymuint i = 0; i < n2; ++ i) {
    const CellGroup* group = cell_class->cell_group(i);
    PyObject* obj = group_list[group->id()];
    Py_INCREF(obj);
    PyList_SetItem(self->mCellGroupList, i, obj);
  }

  return (PyObject*)self;
}

// @brief PyObject から CellClass へのポインタを取り出す．
// @param[in] py_obj Python オブジェクト
// @return CellClass へのポインタを返す．
// @note 変換が失敗したら TypeError を送出し，NULL を返す．
const CellClass*
PyCellClass_AsCellClassPtr(PyObject* py_obj)
{
  // 型のチェック
  if ( !PyCellClass_Check(py_obj) ) {
    PyErr_SetString(PyExc_TypeError, "cell_lib.CellClass is expected");
    return NULL;
  }

  // 強制的にキャスト
  CellClassObject* my_obj = (CellClassObject*)py_obj;

  return my_obj->mClass;
}

// CellClassObject 関係の初期化を行う．
void
CellClassObject_init(PyObject* m)
{
  // タイプオブジェクトの初期化
  if ( PyType_Ready(&PyCellClass_Type) < 0 ) {
    return;
  }

  // タイプオブジェクトの登録
  PyModule_AddObject(m, "CellClass", (PyObject*)&PyCellClass_Type);
}

END_NAMESPACE_YM
