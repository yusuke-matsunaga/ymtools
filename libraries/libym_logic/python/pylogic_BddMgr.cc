
/// @file pylogic_BddMgr.cc
/// @brief BddMgr の Python 用ラッパ
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2013 Yusuke Matsunaga
/// All rights reserved.


#include "ym_logic/pylogic.h"
#include "ym_logic/BddMgr.h"
#include "ym_logic/Bdd.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// Python 用の構造体定義
//////////////////////////////////////////////////////////////////////

// BddMgr を表す型
struct BddMgrObject
{
  // Python のお約束
  PyObject_HEAD

  // BddMgr の本体
  BddMgr* mMgr;

};


//////////////////////////////////////////////////////////////////////
// Python 用のメソッド関数定義
//////////////////////////////////////////////////////////////////////

// BddMgrObject の生成関数
BddMgrObject*
BddMgr_new(PyTypeObject* type)
{
  BddMgrObject* self = PyObject_New(BddMgrObject, type);
  if ( self == NULL ) {
    return NULL;
  }

  // BddMgr を生成する．
  self->mMgr = NULL;

  return self;
}

// BddMgrObject を開放する関数
void
BddMgr_dealloc(BddMgrObject* self)
{
  // BddMgr を開放する．
  delete self->mMgr;

  PyObject_Del(self);
}

// 初期化関数
int
BddMgr_init(BddMgrObject* self,
	    PyObject* args)
{
  char* str1 = NULL;
  char* str2 = NULL;
  char* str3 = NULL;
  if ( !PyArg_ParseTuple(args, "s|ss", &str1, &str2, &str3) ) {
    return -1;
  }

  if ( self->mMgr != NULL ) {
    delete self->mMgr;
  }

  string type_str(str1);
  string name_str;
  string option_str;
  if ( str2 != NULL ) {
    name_str = str2;
  }
  if ( str3 != NULL ) {
    option_str = str3;
  }
  self->mMgr = new BddMgr(type_str, name_str, option_str);

  return 0;
}

// make_zero 関数
// 引数はなし．
PyObject*
BddMgr_make_zero(BddMgrObject* self,
		 PyObject* args)
{
  return PyBdd_FromBdd(self->mMgr->make_zero());
}

// make_one 関数
// 引数はなし．
PyObject*
BddMgr_make_one(BddMgrObject* self,
		PyObject* args)
{
  return PyBdd_FromBdd(self->mMgr->make_one());
}

// make_overflow 関数
// 引数はなし．
PyObject*
BddMgr_make_overflow(BddMgrObject* self,
		     PyObject* args)
{
  return PyBdd_FromBdd(self->mMgr->make_overflow());
}

// make_error 関数
// 引数はなし．
PyObject*
BddMgr_make_error(BddMgrObject* self,
		  PyObject* args)
{
  return PyBdd_FromBdd(self->mMgr->make_error());
}

// make_literal 関数
// Literal か (VarId, Pol) を引数にとる．
PyObject*
BddMgr_make_literal(BddMgrObject* self,
		    PyObject* args)
{
  PyObject* obj1 = NULL;
  PyObject* obj2 = NULL;
  if ( !PyArg_ParseTuple(args, "O|O", &obj1, &obj2) ) {
    return NULL;
  }

  Literal lit;
  if ( obj2 != NULL ) {
    // obj1 は VarId, obj2 は Pol でなければならない．
    if ( !PyVarId_Check(obj1) || !PyPol_Check(obj2) ) {
      PyErr_SetString(PyExc_TypeError, "(logic.VarId, logic.Pol) is expected");
      return NULL;
    }
    VarId vid = PyVarId_AsVarId(obj1);
    tPol pol = PyPol_AsPol(obj2);
    lit.set(vid, pol);
  }
  else {
    if ( !PyLiteral_Check(obj1) ) {
      PyErr_SetString(PyExc_TypeError, "logic.Literal is expected");
      return NULL;
    }
    lit = PyLiteral_AsLiteral(obj1);
  }

  return PyBdd_FromBdd(self->mMgr->make_literal(lit));
}

// make_posiliteral 関数
// VarId を引数にとる．
PyObject*
BddMgr_make_posiliteral(BddMgrObject* self,
			PyObject* args)
{
  PyObject* obj = NULL;
  if ( !PyArg_ParseTuple(args, "O!",
			 &PyVarId_Type, &obj) ) {
    return NULL;
  }
  VarId vid = PyVarId_AsVarId(obj);

  return PyBdd_FromBdd(self->mMgr->make_posiliteral(vid));
}

// make_negaliteral 関数
// VarId を引数にとる．
PyObject*
BddMgr_make_negaliteral(BddMgrObject* self,
			PyObject* args)
{
  PyObject* obj = NULL;
  if ( !PyArg_ParseTuple(args, "O!",
			 &PyVarId_Type, &obj) ) {
    return NULL;
  }
  VarId vid = PyVarId_AsVarId(obj);

  return PyBdd_FromBdd(self->mMgr->make_negaliteral(vid));
}

// make_bdd 関数
// (VarId, Bdd, Bdd) を引数にとる．
PyObject*
BddMgr_make_bdd(BddMgrObject* self,
		PyObject* args)
{
  PyObject* obj1 = NULL;
  PyObject* obj2 = NULL;
  PyObject* obj3 = NULL;
  if ( !PyArg_ParseTuple(args, "O!O!O!",
			 &PyVarId_Type, &obj1,
			 &PyBdd_Type, &obj2,
			 &PyBdd_Type, &obj3) ) {
    return NULL;
  }

  VarId vid = PyVarId_AsVarId(obj1);
  Bdd bdd0 = PyBdd_AsBdd(obj2);
  Bdd bdd1 = PyBdd_AsBdd(obj3);

  return PyBdd_FromBdd(self->mMgr->make_bdd(vid, bdd0, bdd1));
}

// tvec_to_bdd 関数
// (int のタプル, VarId のタプル) を引数にとる．
PyObject*
BddMgr_tvec_to_bdd(BddMgrObject* self,
		   PyObject* args)
{
  PyObject* obj1 = NULL;
  PyObject* obj2 = NULL;
  if ( !PyArg_ParseTuple(args, "O!O!",
			 &PyTuple_Type, &obj1,
			 &PyTuple_Type, &obj2) ) {
    return NULL;
  }

  ymuint nvec = PyTuple_GET_SIZE(obj1);
  vector<int> vec(nvec);
  for (ymuint i = 0; i < nvec; ++ i) {
    PyObject* val_obj = PyTuple_GET_ITEM(obj1, i);
    int val;
    if ( !PyArg_ParseTuple(val_obj, "i", &val) ) {
      return NULL;
    }
    vec[i] = val;
  }

  ymuint nv = PyTuple_GET_SIZE(obj2);
  if ( nvec != (1U << nv) ) {
    PyErr_SetString(PyExc_TypeError, "size mismatch");
    return NULL;
  }
  VarVector vars(nv);
  for (ymuint i = 0; i < nv; ++ i) {
    PyObject* var_obj = PyTuple_GET_ITEM(obj2, i);
    if ( !PyVarId_Check(var_obj) ) {
      PyErr_SetString(PyExc_TypeError, "tuple of logic.VarIds is expected");
      return NULL;
    }
    vars[i] = PyVarId_AsVarId(var_obj);
  }

  return PyBdd_FromBdd(self->mMgr->tvec_to_bdd(vec, vars));
}

// expr_to_bdd 関数
// (LogExpr, VarId : VarId のディクショナリ) を引数にとる．
PyObject*
BddMgr_expr_to_bdd(BddMgrObject* self,
		   PyObject* args)
{
  PyObject* obj1 = NULL;
  PyObject* obj2 = NULL;
  if ( !PyArg_ParseTuple(args, "O!|O!",
			 &PyLogExpr_Type, &obj1,
			 &PyDict_Type, &obj2) ) {
    return NULL;
  }

  LogExpr* expr_p = PyLogExpr_AsLogExprPtr(obj1);

  VarVarMap varmap;
  if ( obj2 != NULL ) {
    PyObject* item_list = PyDict_Items(obj2);
    ymuint n = PyList_GET_SIZE(item_list);
    for (ymuint i = 0; i < n; ++ i) {
      PyObject* item = PyList_GET_ITEM(item_list, i);
      assert_cond( PyTuple_Check(item) && PyTuple_GET_SIZE(item) == 2, __FILE__, __LINE__);

      PyObject* vid1_obj = PyTuple_GET_ITEM(item, 0);
      PyObject* vid2_obj = PyTuple_GET_ITEM(item, 1);
      if ( !PyVarId_Check(vid1_obj) || PyVarId_Check(vid2_obj) ) {
	PyErr_SetString(PyExc_TypeError,
			"A dictionary of (VarId, VarId) is expected");
	return NULL;
      }
      VarId vid1 = PyVarId_AsVarId(vid1_obj);
      VarId vid2 = PyVarId_AsVarId(vid2_obj);

      varmap.add(vid1, vid2);
    }
  }

  return PyBdd_FromBdd(self->mMgr->expr_to_bdd(*expr_p, varmap));
}

// make_thfunc 関数
PyObject*
BddMgr_make_thfunc(BddMgrObject* self,
		   PyObject* args)
{
  ymuint n;
  ymuint th;
  if ( !PyArg_ParseTuple(args, "kk", &n, &th) ) {
    return NULL;
  }

  return PyBdd_FromBdd(self->mMgr->make_thfunc(n, th));
}

// new_var 関数
PyObject*
BddMgr_new_var(BddMgrObject* self,
	       PyObject* args)
{
  PyObject* obj = NULL;
  if ( !PyArg_ParseTuple(args, "O!",
			 &PyVarId_Type, &obj) ) {
    return NULL;
  }

  VarId vid= PyVarId_AsVarId(obj);

  bool stat = self->mMgr->new_var(vid);

  return PyObject_FromBool(stat);
}

// var_list 関数
PyObject*
BddMgr_var_list(BddMgrObject* self,
		PyObject* args)
{
  list<VarId> vlist;
  self->mMgr->var_list(vlist);

  ymuint n = vlist.size();
  PyObject* ans_list = PyList_New(n);
  ymuint i = 0;
  for (list<VarId>::iterator p = vlist.begin();
       p != vlist.end(); ++ p, ++ i) {
    VarId vid = *p;
    PyObject* obj = PyVarId_FromVarId(vid);
    PyList_SetItem(ans_list, i, obj);
  }

  return ans_list;
}

// level 関数
PyObject*
BddMgr_level(BddMgrObject* self,
	     PyObject* args)
{
  PyObject* obj = NULL;
  if ( !PyArg_ParseTuple(args, "O!",
			 &PyVarId_Type, &obj) ) {
    return NULL;
  }

  VarId vid = PyVarId_AsVarId(obj);

  return PyObject_FromYmuint32(self->mMgr->level(vid));
}

// varid 関数
PyObject*
BddMgr_varid(BddMgrObject* self,
	     PyObject* args)
{
  ymuint level;
  if ( !PyArg_ParseTuple(args, "k", &level) ) {
    return NULL;
  }

  VarId vid = self->mMgr->varid(level);
  return PyVarId_FromVarId(vid);
}

// enable_gc 関数
PyObject*
BddMgr_enable_gc(BddMgrObject* self,
		 PyObject* args)
{
  self->mMgr->enable_gc();

  Py_INCREF(Py_None);
  return Py_None;
}

// disable_gc 関数
PyObject*
BddMgr_disable_gc(BddMgrObject* self,
		  PyObject* args)
{
  self->mMgr->disable_gc();

  Py_INCREF(Py_None);
  return Py_None;
}

// gc 関数
PyObject*
BddMgr_gc(BddMgrObject* self,
	  PyObject* args)
{
  bool shrink;
  if ( !PyArg_ParseTuple(args, "b", &shrink) ) {
    return NULL;
  }

  self->mMgr->gc(shrink);

  Py_INCREF(Py_None);
  return Py_None;
}

// name 関数
PyObject*
BddMgr_name(BddMgrObject* self,
	    PyObject* args)
{
  return PyObject_FromString(self->mMgr->name());
}

// BddMgrObject のメソッドテーブル
PyMethodDef BddMgr_methods[] = {
  {"make_zero", (PyCFunction)BddMgr_make_zero, METH_NOARGS,
   PyDoc_STR("make constant zero (NONE)")},
  {"make_one", (PyCFunction)BddMgr_make_one, METH_NOARGS,
   PyDoc_STR("make constant one (NONE)")},
  {"make_overflow", (PyCFunction)BddMgr_make_overflow, METH_NOARGS,
   PyDoc_STR("make overflow node (NONE)")},
  {"make_error", (PyCFunction)BddMgr_make_error, METH_NOARGS,
   PyDoc_STR("make error node zero (NONE)")},
  {"make_literal", (PyCFunction)BddMgr_make_literal, METH_VARARGS,
   PyDoc_STR("make literal function (Literal|VarId|(VarId, Pol))")},
  {"make_posiliteral", (PyCFunction)BddMgr_make_posiliteral, METH_VARARGS,
   PyDoc_STR("make positive literal function (VarId)")},
  {"make_negaliteral", (PyCFunction)BddMgr_make_negaliteral, METH_VARARGS,
   PyDoc_STR("make negative literal function (VarId)")},
  {"make_bdd", (PyCFunction)BddMgr_make_bdd, METH_VARARGS,
   PyDoc_STR("make bdd (VarId, Bdd, Bdd)")},
  {"tvec_to_bdd", (PyCFunction)BddMgr_tvec_to_bdd, METH_VARARGS,
   PyDoc_STR("make bdd from truth vector (tuple of int, tuple of VarId)")},
  {"expr_to_bdd", (PyCFunction)BddMgr_expr_to_bdd, METH_VARARGS,
   PyDoc_STR("make bdd from expr (LogExpr, dictionary of VarId : VarId)")},
  {"make_thfunc", (PyCFunction)BddMgr_make_thfunc, METH_VARARGS,
   PyDoc_STR("make threshold function (uint, uint)")},
  {"new_var", (PyCFunction)BddMgr_new_var, METH_VARARGS,
   PyDoc_STR("make new variable (VarId)")},
  {"var_list", (PyCFunction)BddMgr_var_list, METH_NOARGS,
   PyDoc_STR("return variable list (NONE)")},
  {"level", (PyCFunction)BddMgr_level, METH_VARARGS,
   PyDoc_STR("return level of variable (VarId)")},
  {"varid", (PyCFunction)BddMgr_varid, METH_VARARGS,
   PyDoc_STR("return variable of the level (uint)")},
  {"enable_gc", (PyCFunction)BddMgr_enable_gc, METH_NOARGS,
   PyDoc_STR("enable GC")},
  {"disable_gc", (PyCFunction)BddMgr_disable_gc, METH_NOARGS,
   PyDoc_STR("disable GC")},
  {"gc", (PyCFunction)BddMgr_gc, METH_VARARGS,
   PyDoc_STR("do GC")},
  {NULL, NULL, 0, NULL}
};

END_NONAMESPACE


// BddMgrObject 用のタイプオブジェクト
PyTypeObject PyBddMgr_Type = {
  /* The ob_type field must be initialized in the module init function
   * to be portable to Windows without using C++. */
  PyVarObject_HEAD_INIT(NULL, 0)
  "logic.BddMgr",            /*tp_name*/
  sizeof(BddMgrObject),      /*tp_basicsize*/
  0,                          /*tp_itemsize*/
  /* methods */
  (destructor)BddMgr_dealloc,    /*tp_dealloc*/
  0,                          /*tp_print*/
  0,                          /*tp_getattr*/
  0,                          /*tp_setattr*/
  0,                          /*tp_compare*/
  0,                          /*tp_repr*/
  0,                          /*tp_as_number*/
  0,                          /*tp_as_sequence*/
  0,                          /*tp_as_mapping*/
  0,                          /*tp_hash*/
  0,                          /*tp_call*/
  0,                          /*tp_str*/
  0,                          /*tp_getattro*/
  0,                          /*tp_setattro*/
  0,                          /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,         /*tp_flags*/
  "BDD manager",              /*tp_doc*/
  0,                          /*tp_traverse*/
  0,                          /*tp_clear*/
  0,                          /*tp_richcompare*/
  0,                          /*tp_weaklistoffset*/
  0,                          /*tp_iter*/
  0,                          /*tp_iternext*/
  BddMgr_methods,             /*tp_methods*/
  0,                          /*tp_members*/
  0,                          /*tp_getset*/
  0,                          /*tp_base*/
  0,                          /*tp_dict*/
  0,                          /*tp_descr_get*/
  0,                          /*tp_descr_set*/
  0,                          /*tp_dictoffset*/
  (initproc)BddMgr_init,      /*tp_init*/
  0,                          /*tp_alloc*/
  (newfunc)BddMgr_new,        /*tp_new*/
  0,                          /*tp_free*/
  0,                          /*tp_is_gc*/
};

// @brief PyObject から BddMgr へのポインタを取り出す．
// @param[in] py_obj Python オブジェクト
// @return BddMgr へのポインタを返す．
// @note 変換が失敗したら TypeError を送出し，NULL を返す．
BddMgr*
PyBddMgr_AsBddMgrPtr(PyObject* py_obj)
{
  // 型のチェック
  if ( !PyBddMgr_Check(py_obj) ) {
    PyErr_SetString(PyExc_TypeError, "logic.BddMgr is expected");
    return NULL;
  }

  // 強制的にキャスト
  BddMgrObject* bddmgr_obj = (BddMgrObject*)py_obj;

  return bddmgr_obj->mMgr;
}

// BddMgrObject 関係の初期化を行う．
void
BddMgrObject_init(PyObject* m)
{
  // タイプオブジェクトの初期化
  if ( PyType_Ready(&PyBddMgr_Type) < 0 ) {
    return;
  }

  // タイプオブジェクトの登録
  PyModule_AddObject(m, "BddMgr", (PyObject*)&PyBddMgr_Type);
}

END_NAMESPACE_YM
