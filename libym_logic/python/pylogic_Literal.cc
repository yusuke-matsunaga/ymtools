﻿
/// @file pylogic_Literal.cc
/// @brief Literal の Python 用ラッパ
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2013 Yusuke Matsunaga
/// All rights reserved.


#include "YmLogic/pylogic.h"
#include "YmLogic/Literal.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// Python 用の構造体定義
//////////////////////////////////////////////////////////////////////

// Literal を表す型
struct LiteralObject
{
  // Python のお約束
  PyObject_HEAD

  // Literal 本体
  Literal mLiteral;

};


//////////////////////////////////////////////////////////////////////
// Python 用のメソッド関数定義
//////////////////////////////////////////////////////////////////////

// LiteralObject の生成関数
LiteralObject*
Literal_new(PyTypeObject* type)
{
  LiteralObject* self = PyObject_New(LiteralObject, type);
  if ( self == NULL ) {
    return NULL;
  }

  // mLiteral は生成されている．

  return self;
}

// LiteralObject を開放する関数
void
Literal_dealloc(LiteralObject* self)
{
  // mLiteral は開放の必要がない．

  PyObject_Del(self);
}

// 比較関数
int
Literal_compare(LiteralObject* left,
		LiteralObject* right)
{
  if ( left->mLiteral < right->mLiteral ) {
    return -1;
  }
  if ( left->mLiteral > right->mLiteral ) {
    return 1;
  }
  return 0;
}

// 初期化関数
// VarId と Pol を引数にとりうる．
// 省略された場合には 0 番の kPolPosi で初期化される．
int
Literal_init(LiteralObject* self,
	     PyObject* args)
{
  PyObject* vobj = NULL;
  PyObject* pobj = NULL;
  if ( !PyArg_ParseTuple(args, "|O!O!",
			 &PyVarId_Type, &vobj,
			 &PyPol_Type, &pobj) ) {
    return -1;
  }
  VarId vid;
  if ( vobj != NULL ) {
    vid = PyVarId_AsVarId(vobj);
  }
  else {
    vid = VarId(0);
  }

  tPol pol = kPolPosi;
  if ( pobj != NULL ) {
    pol = PyPol_AsPol(pobj);
  }

  self->mLiteral.set(vid, pol);

  return 0;
}

// str 関数
PyObject*
Literal_str(LiteralObject* self)
{
  VarId vid = self->mLiteral.varid();
  tPol pol = self->mLiteral.pol();

  ostringstream buf;
  if ( pol == kPolNega ) {
    buf << "~";
  }
  buf << "V" << vid.val();
  return PyObject_FromString(buf.str());
}

// hash 関数
long
Literal_hash(LiteralObject* self)
{
  return self->mLiteral.hash();
}

// varid 関数
PyObject*
Literal_varid(LiteralObject* self)
{
  VarId vid = self->mLiteral.varid();
  return PyVarId_FromVarId(vid);
}

// pol 関数
PyObject*
Literal_pol(LiteralObject* self)
{
  tPol pol = self->mLiteral.pol();
  return PyPol_FromPol(pol);
}

// set 関数
// VarId と Pol を引数にとる．
// Pol は省略可能で省略時には kPolPosi が仮定される．
PyObject*
Literal_set(LiteralObject* self,
	    PyObject* args)
{
  PyObject* vobj = NULL;
  PyObject* pobj = NULL;
  if ( !PyArg_ParseTuple(args, "O!|O!",
			 &PyVarId_Type, &vobj,
			 &PyPol_Type, &pobj) ) {
    return NULL;
  }
  VarId vid = PyVarId_AsVarId(vobj);

  tPol pol = kPolPosi;
  if ( pobj != NULL ) {
    pol = PyPol_AsPol(pobj);
  }

  self->mLiteral.set(vid, pol);

  Py_INCREF(Py_None);
  return Py_None;
}

// negate 関数
// 極性が逆のリテラルを返す．
PyObject*
Literal_negate(LiteralObject* self)
{
  return PyLiteral_FromLiteral(~self->mLiteral);
}

// make_positive 関数
PyObject*
Literal_make_positive(LiteralObject* self)
{
  return PyLiteral_FromLiteral(self->mLiteral.make_positive());
}

// make_negative 関数
PyObject*
Literal_make_negative(LiteralObject* self)
{
  return PyLiteral_FromLiteral(self->mLiteral.make_negative());
}

// index 関数
PyObject*
Literal_index(LiteralObject* self)
{
  return PyObject_FromYmuint32(self->mLiteral.index());
}

// LiteralObject のメソッドテーブル
PyMethodDef Literal_methods[] = {
  {"varid", (PyCFunction)Literal_varid, METH_NOARGS,
   PyDoc_STR("return Literal::varid()")},
  {"pol", (PyCFunction)Literal_pol, METH_NOARGS,
   PyDoc_STR("return Literal::pol()")},
  {"set", (PyCFunction)Literal_set, METH_VARARGS,
   PyDoc_STR("set Literal")},
  {"negate", (PyCFunction)Literal_negate, METH_NOARGS,
   PyDoc_STR("return negated Literal")},
  {"make_positive", (PyCFunction)Literal_make_positive, METH_NOARGS,
   PyDoc_STR("return positive Literal")},
  {"make_negative", (PyCFunction)Literal_make_negative, METH_NOARGS,
   PyDoc_STR("return negative Literal")},
  {"index", (PyCFunction)Literal_index, METH_NOARGS,
   PyDoc_STR("return Literal::index()")},
  {NULL, NULL, 0, NULL}
};

END_NONAMESPACE


// LiteralObject 用のタイプオブジェクト
PyTypeObject PyLiteral_Type = {
  /* The ob_type field must be initialized in the module init function
   * to be portable to Windows without using C++. */
  PyVarObject_HEAD_INIT(NULL, 0)
  "logic.Literal",            /*tp_name*/
  sizeof(LiteralObject),      /*tp_basicsize*/
  0,                          /*tp_itemsize*/
  /* methods */
  (destructor)Literal_dealloc,  /*tp_dealloc*/
  0,                          /*tp_print*/
  0,                          /*tp_getattr*/
  0,                          /*tp_setattr*/
  (cmpfunc)Literal_compare,     /*tp_compare*/
  0,                          /*tp_repr*/
  0,                          /*tp_as_number*/
  0,                          /*tp_as_sequence*/
  0,                          /*tp_as_mapping*/
  (hashfunc)Literal_hash,     /*tp_hash*/
  0,                          /*tp_call*/
  (reprfunc)Literal_str,      /*tp_str*/
  0,                          /*tp_getattro*/
  0,                          /*tp_setattro*/
  0,                          /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,         /*tp_flags*/
  "Literal",                  /*tp_doc*/
  0,                          /*tp_traverse*/
  0,                          /*tp_clear*/
  0,                          /*tp_richcompare*/
  0,                          /*tp_weaklistoffset*/
  0,                          /*tp_iter*/
  0,                          /*tp_iternext*/
  Literal_methods,            /*tp_methods*/
  0,                          /*tp_members*/
  0,                          /*tp_getset*/
  0,                          /*tp_base*/
  0,                          /*tp_dict*/
  0,                          /*tp_descr_get*/
  0,                          /*tp_descr_set*/
  0,                          /*tp_dictoffset*/
  (initproc)Literal_init,     /*tp_init*/
  0,                          /*tp_alloc*/
  (newfunc)Literal_new,       /*tp_new*/
  0,                          /*tp_free*/
  0,                          /*tp_is_gc*/
};

// @brief Literal から PyObject を生成する．
// @param[in] obj Literal オブジェクト
PyObject*
PyLiteral_FromLiteral(Literal obj)
{
  LiteralObject* literal_obj = Literal_new(&PyLiteral_Type);
  if ( literal_obj == NULL ) {
    return NULL;
  }

  literal_obj->mLiteral = obj;

  Py_INCREF(literal_obj);
  return (PyObject*)literal_obj;
}

// @brief PyObject から Literal を取り出す．
// @param[in] py_obj Python オブジェクト
// @return Literal を返す．
// @note 変換が失敗したら TypeError を送出し，Literal(VarId(0), kPolPosi) を返す．
Literal
PyLiteral_AsLiteral(PyObject* py_obj)
{
  // 型のチェック
  if ( !PyLiteral_Check(py_obj) ) {
    PyErr_SetString(PyExc_TypeError, "logic.Literal is expected");
    return Literal();
  }

  // 強制的にキャスト
  LiteralObject* literal_obj = (LiteralObject*)py_obj;

  return literal_obj->mLiteral;
}

// LiteralObject 関係の初期化を行う．
void
LiteralObject_init(PyObject* m)
{
  // タイプオブジェクトの初期化
  if ( PyType_Ready(&PyLiteral_Type) < 0 ) {
    return;
  }

  // タイプオブジェクトの登録
  PyModule_AddObject(m, "Literal", (PyObject*)&PyLiteral_Type);
}

END_NAMESPACE_YM
