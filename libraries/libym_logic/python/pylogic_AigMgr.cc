
/// @file pylogic_AigMgr.cc
/// @brief AigMgr の Python 用ラッパ
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2013 Yusuke Matsunaga
/// All rights reserved.


#include "ym_logic/pylogic.h"
#include "ym_logic/AigMgr.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// Python 用の構造体定義
//////////////////////////////////////////////////////////////////////

// AigMgr を表す型
struct AigMgrObject
{
  // Python のお約束
  PyObject_HEAD

  // AigMgr の本体
  AigMgr* mMgr;

};


//////////////////////////////////////////////////////////////////////
// Python 用のメソッド関数定義
//////////////////////////////////////////////////////////////////////

// AigMgrObject の生成関数
AigMgrObject*
AigMgr_new(PyTypeObject* type)
{
  AigMgrObject* self = PyObject_New(AigMgrObject, type);
  if ( self == NULL ) {
    return NULL;
  }

  // AigMgr を生成する．
  self->mMgr = new AigMgr;

  return self;
}

// AigMgrObject を開放する関数
void
AigMgr_dealloc(AigMgrObject* self)
{
  // AigMgr を開放する．
  delete self->mMgr;

  PyObject_Del(self);
}

// 初期化関数
int
AigMgr_init(AigMgrObject* self,
	    PyObject* args)
{
  // なにもしない．
  return 0;
}

// input_num 関数
PyObject*
AigMgr_input_num(AigMgrObject* self,
		 PyObject* args)
{
  return PyObject_FromYmuint32(self->mMgr->input_num());
}

// node_num 関数
PyObject*
AigMgr_node_num(AigMgrObject* self,
		PyObject* args)
{
  return PyObject_FromYmuint32(self->mMgr->node_num());
}

// print_handles 関数
PyObject*
AigMgr_print_handles(AigMgrObject* self,
		     PyObject* args)
{
#warning "TODO: 未完"
  return NULL;
}

// make_zero 関数
PyObject*
AigMgr_make_zero(AigMgrObject* self,
		 PyObject* args)
{
  return PyAig_FromAig(self->mMgr->make_zero());
}

// make_one 関数
PyObject*
AigMgr_make_one(AigMgrObject* self,
		PyObject* args)
{
  return PyAig_FromAig(self->mMgr->make_one());
}

// make_input 関数
PyObject*
AigMgr_make_input(AigMgrObject* self,
		  PyObject* args)
{
  PyObject* obj = NULL;
  if ( !PyArg_ParseTuple(args, "O!",
			 &PyVarId_Type, &obj) ) {
    return NULL;
  }

  VarId vid = PyVarId_AsVarId(obj);

  return PyAig_FromAig(self->mMgr->make_input(vid));
}

// make_not 関数
PyObject*
AigMgr_make_not(AigMgrObject* self,
		PyObject* args)
{
  PyObject* obj = NULL;
  if ( !PyArg_ParseTuple(args, "O!",
			 &PyAig_Type, &obj) ) {
    return NULL;
  }

  Aig aig = PyAig_AsAig(obj);

  return PyAig_FromAig(self->mMgr->make_not(aig));
}

BEGIN_NONAMESPACE

// 引数を Aig のタプルかリストだと思ってパーズする．
bool
parse_aig_list(PyObject* args,
	       vector<Aig>& aig_list)
{
  ymuint n = PyTuple_GET_SIZE(args);
  aig_list.clear();
  aig_list.reserve(n);
  if ( n == 1 ) {
    PyObject* obj0 = PyTuple_GET_ITEM(args, 0);
    if ( !PyList_Check(obj0) ) {
      PyErr_SetString(PyExc_TypeError, "list of Aigs is expected");
      return false;
    }
    n = PyList_GET_SIZE(obj0);
    aig_list.reserve(n);
    for (ymuint i = 0; i < n; ++ i) {
      PyObject* obj = PyList_GET_ITEM(obj0, i);
      if ( !PyAig_Check(obj) ) {
	PyErr_SetString(PyExc_TypeError, "list of Aigs is expected");
	return false;
      }
      Aig aig = PyAig_AsAig(obj);
      aig_list.push_back(aig);
    }
  }
  else {
    for (ymuint i = 0; i < n; ++ i) {
      PyObject* obj = PyTuple_GET_ITEM(args, i);
      if ( !PyAig_Check(obj) ) {
	PyErr_SetString(PyExc_TypeError, "tuple of Aigs is expected");
	return false;
      }
      Aig aig = PyAig_AsAig(obj);
      aig_list.push_back(aig);
    }
  }
  return true;
}

END_NONAMESPACE

// make_and 関数
PyObject*
AigMgr_make_and(AigMgrObject* self,
		PyObject* args)
{
  vector<Aig> aig_list;
  if ( !parse_aig_list(args, aig_list) ) {
    return NULL;
  }

  return PyAig_FromAig(self->mMgr->make_and(aig_list));
}

// make_nand 関数
PyObject*
AigMgr_make_nand(AigMgrObject* self,
		 PyObject* args)
{
  vector<Aig> aig_list;
  if ( !parse_aig_list(args, aig_list) ) {
    return NULL;
  }

  return PyAig_FromAig(self->mMgr->make_nand(aig_list));
}

// make_or 関数
PyObject*
AigMgr_make_or(AigMgrObject* self,
	       PyObject* args)
{
  vector<Aig> aig_list;
  if ( !parse_aig_list(args, aig_list) ) {
    return NULL;
  }

  return PyAig_FromAig(self->mMgr->make_or(aig_list));
}

// make_nor 関数
PyObject*
AigMgr_make_nor(AigMgrObject* self,
		PyObject* args)
{
  vector<Aig> aig_list;
  if ( !parse_aig_list(args, aig_list) ) {
    return NULL;
  }

  return PyAig_FromAig(self->mMgr->make_nor(aig_list));
}

// make_xor 関数
PyObject*
AigMgr_make_xor(AigMgrObject* self,
		PyObject* args)
{
  vector<Aig> aig_list;
  if ( !parse_aig_list(args, aig_list) ) {
    return NULL;
  }

  return PyAig_FromAig(self->mMgr->make_xor(aig_list));
}

// make_xnor 関数
PyObject*
AigMgr_make_xnor(AigMgrObject* self,
		 PyObject* args)
{
  vector<Aig> aig_list;
  if ( !parse_aig_list(args, aig_list) ) {
    return NULL;
  }

  return PyAig_FromAig(self->mMgr->make_xnor(aig_list));
}

// make_logic 関数
PyObject*
AigMgr_make_logic(AigMgrObject* self,
		  PyObject* args)
{
  PyObject* obj1 = NULL;
  PyObject* obj2 = NULL;
  if ( !PyArg_ParseTuple(args, "O!O!",
			 &PyLogExpr_Type, &obj1,
			 &PyDict_Type, &obj2) ) {
    return NULL;
  }

  LogExpr* expr_p = PyLogExpr_AsLogExprPtr(obj1);

  HashMap<VarId, Aig> input_map;
  PyObject* item_list = PyDict_Items(obj2);
  ymuint n = PyList_GET_SIZE(item_list);
  for (ymuint i = 0; i < n; ++ i) {
    PyObject* item = PyList_GET_ITEM(item_list, i);
    PyObject* vid_obj = NULL;
    PyObject* aig_obj = NULL;
    if ( !PyArg_ParseTuple(item, "O!O!",
			   &PyVarId_Type, &vid_obj,
			   &PyAig_Type, &aig_obj) ) {
      return NULL;
    }

    VarId vid = PyVarId_AsVarId(vid_obj);
    Aig aig = PyAig_AsAig(aig_obj);

    input_map.add(vid, aig);
  }

  return PyAig_FromAig(self->mMgr->make_logic(*expr_p, input_map));
}

// make_cofactor 関数
PyObject*
AigMgr_make_cofactor(AigMgrObject* self,
		     PyObject* args)
{
  PyObject* obj1 = NULL;
  PyObject* obj2 = NULL;
  PyObject* obj3 = NULL;
  if ( !PyArg_ParseTuple(args, "O!O!O!",
			 &PyAig_Type, &obj1,
			 &PyVarId_Type, &obj2,
			 &PyPol_Type, &obj3) ) {
    return NULL;
  }

  Aig aig = PyAig_AsAig(obj1);
  VarId vid = PyVarId_AsVarId(obj2);
  tPol pol = PyPol_AsPol(obj3);

  return PyAig_FromAig(self->mMgr->make_cofactor(aig, vid, pol));
}

// AigMgrObject のメソッドテーブル
PyMethodDef AigMgr_methods[] = {
  {"input_num", (PyCFunction)AigMgr_input_num, METH_NOARGS,
   PyDoc_STR("return the number of inputs (NONE)")},
  {"node_num", (PyCFunction)AigMgr_node_num, METH_NOARGS,
   PyDoc_STR("return the number of nodes (NONE)")},
  {"make_zero", (PyCFunction)AigMgr_make_zero, METH_NOARGS,
   PyDoc_STR("make constant 0 (NONE)")},
  {"make_one", (PyCFunction)AigMgr_make_one, METH_NOARGS,
   PyDoc_STR("make constant 1 (NONE)")},
  {"make_input", (PyCFunction)AigMgr_make_input, METH_VARARGS,
   PyDoc_STR("make primary input (VarId)")},
  {"make_not", (PyCFunction)AigMgr_make_not, METH_VARARGS,
   PyDoc_STR("make NOT node (Aig)")},
  {"make_and", (PyCFunction)AigMgr_make_and, METH_VARARGS,
   PyDoc_STR("make AND node (list|tuple of Aig)")},
  {"make_nand", (PyCFunction)AigMgr_make_nand, METH_VARARGS,
   PyDoc_STR("make NAND node (list|tuple of Aig)")},
  {"make_or", (PyCFunction)AigMgr_make_or, METH_VARARGS,
   PyDoc_STR("make OR node (list|tuple of Aig)")},
  {"make_nor", (PyCFunction)AigMgr_make_nor, METH_VARARGS,
   PyDoc_STR("make NOR node (list|tuple of Aig)")},
  {"make_xor", (PyCFunction)AigMgr_make_xor, METH_VARARGS,
   PyDoc_STR("make XOR node (list|tuple of Aig)")},
  {"make_xnor", (PyCFunction)AigMgr_make_xnor, METH_VARARGS,
   PyDoc_STR("make XNOR node (list|tuple of Aig)")},
  {"make_logic", (PyCFunction)AigMgr_make_logic, METH_NOARGS,
   PyDoc_STR("make AIG from a logic expression (LogExpr, dictionary of ( VarId : Aig ))")},
  {"make_cofactor", (PyCFunction)AigMgr_make_cofactor, METH_NOARGS,
   PyDoc_STR("make cofactor (Aig, VarId, Pol)")},
  {NULL, NULL, 0, NULL}
};

END_NONAMESPACE


// AigMgrObject 用のタイプオブジェクト
PyTypeObject PyAigMgr_Type = {
  /* The ob_type field must be initialized in the module init function
   * to be portable to Windows without using C++. */
  PyVarObject_HEAD_INIT(NULL, 0)
  "logic.AigMgr",            /*tp_name*/
  sizeof(AigMgrObject),      /*tp_basicsize*/
  0,                          /*tp_itemsize*/
  /* methods */
  (destructor)AigMgr_dealloc,    /*tp_dealloc*/
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
  "AigMgr",                   /*tp_doc*/
  0,                          /*tp_traverse*/
  0,                          /*tp_clear*/
  0,                          /*tp_richcompare*/
  0,                          /*tp_weaklistoffset*/
  0,                          /*tp_iter*/
  0,                          /*tp_iternext*/
  AigMgr_methods,                /*tp_methods*/
  0,                          /*tp_members*/
  0,                          /*tp_getset*/
  0,                          /*tp_base*/
  0,                          /*tp_dict*/
  0,                          /*tp_descr_get*/
  0,                          /*tp_descr_set*/
  0,                          /*tp_dictoffset*/
  (initproc)AigMgr_init,         /*tp_init*/
  0,                          /*tp_alloc*/
  (newfunc)AigMgr_new,           /*tp_new*/
  0,                          /*tp_free*/
  0,                          /*tp_is_gc*/
};

/// @brief PyObject から AigMgr へのポインタを取り出す．
/// @param[in] py_obj Python オブジェクト
/// @return AigMgr へのポインタを返す．
/// @note 変換が失敗したら TypeError を送出し，NULL を返す．
AigMgr*
PyAigMgr_AsAigMgrPtr(PyObject* py_obj)
{
  // 型のチェック
  if ( !PyAigMgr_Check(py_obj) ) {
    PyErr_SetString(PyExc_TypeError, "logic.AigMgr type is expected");
    return NULL;
  }

  // 強制的にキャスト
  AigMgrObject* aigmgr_obj = (AigMgrObject*)py_obj;

  return aigmgr_obj->mMgr;
}

// AigMgrObject 関係の初期化を行う．
void
AigMgrObject_init(PyObject* m)
{
  // タイプオブジェクトの初期化
  if ( PyType_Ready(&PyAigMgr_Type) < 0 ) {
    return;
  }

  // タイプオブジェクトの登録
  PyModule_AddObject(m, "AigMgr", (PyObject*)&PyAigMgr_Type);
}

END_NAMESPACE_YM
