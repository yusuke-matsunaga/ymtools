﻿
/// @file pylogic_SatSolver.cc
/// @brief SatSolver の Python 用ラッパ
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2013 Yusuke Matsunaga
/// All rights reserved.


#include "YmLogic/pylogic.h"
#include "YmLogic/SatSolver.h"
#include "YmLogic/SatStats.h"
#include "PySatMsgHandler.h"
#include "YmUtils/pyutils.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// Python 用の構造体定義
//////////////////////////////////////////////////////////////////////

// SatSolver を表す型
struct SatSolverObject
{
  // Python のお約束
  PyObject_HEAD

  // SatSolver の本体
  SatSolver* mSolver;

};


//////////////////////////////////////////////////////////////////////
// Python 用のメソッド関数定義
//////////////////////////////////////////////////////////////////////

// SatSolverObject の生成関数
SatSolverObject*
SatSolver_new(PyTypeObject* type)
{
  SatSolverObject* self = PyObject_New(SatSolverObject, type);
  if ( self == NULL ) {
    return NULL;
  }

  self->mSolver = NULL;

  return self;
}


// SatSolver を開放する関数
void
SatSolver_dealloc(SatSolverObject* self)
{
  delete self->mSolver;

  PyObject_Del(self);
}

// 初期化関数
int
SatSolver_init(SatSolverObject* self,
	       PyObject* args)
{
  char* type_s = NULL;
  char* option_s = NULL;
  char* file_s = NULL;
  if ( !PyArg_ParseTuple(args, "|sss", &type_s, &option_s, &file_s) ) {
    return -1;
  }

  string type;
  if ( type_s != NULL ) {
    type = type_s;
  }

  string option;
  if ( option_s != NULL ) {
    option = option_s;
  }

  // 今は outp は無視

  if ( self->mSolver != NULL ) {
    delete self->mSolver;
  }

  self->mSolver = new SatSolver(type, option);

  return 0;
}

// sane 関数
PyObject*
SatSolver_sane(SatSolverObject* self,
	       PyObject* args)
{
  return PyObject_FromBool(self->mSolver->sane());
}

// new_var 関数
PyObject*
SatSolver_new_var(SatSolverObject* self,
		  PyObject* args)
{
  return PyVarId_FromVarId(self->mSolver->new_var());
}

// add_clause 関数
PyObject*
SatSolver_add_clause(SatSolverObject* self,
		     PyObject* args)
{
  vector<Literal> lits;
  ymuint n = PyTuple_GET_SIZE(args);
  if ( n == 1 ) {
    PyObject* obj0 = PyTuple_GET_ITEM(args, 0);
    if ( !PyList_Check(obj0) ) {
      PyErr_SetString(PyExc_TypeError, "list of logic.Literal is expected");
      return NULL;
    }
    n = PyList_GET_SIZE(obj0);
    lits.resize(n);
    for (ymuint i = 0; i < n; ++ i) {
      PyObject* obj = PyList_GET_ITEM(obj0, i);
      if ( !PyLiteral_Check(obj) ) {
	PyErr_SetString(PyExc_TypeError, "logic.Literal is expected");
	return NULL;
      }

      lits[i] = PyLiteral_AsLiteral(obj);
    }
  }
  else {
    lits.resize(n);
    for (ymuint i = 0; i < n; ++ i) {
      PyObject* obj = PyTuple_GET_ITEM(args, i);
      if ( !PyLiteral_Check(obj) ) {
	PyErr_SetString(PyExc_TypeError, "logic.Literal is expected");
	return NULL;
      }

      lits[i] = PyLiteral_AsLiteral(obj);
    }
  }

  self->mSolver->add_clause(lits);

  Py_INCREF(Py_None);
  return Py_None;
}

// solve 関数
PyObject*
SatSolver_solve(SatSolverObject* self,
		PyObject* args)
{
  vector<Bool3> model;
  Bool3 stat;

  if ( args != NULL ) {
    if ( PyTuple_GET_SIZE(args) != 1 ||
	 !PyList_Check(PyTuple_GET_ITEM(args, 0)) ) {
      PyErr_SetString(PyExc_TypeError, "list of Literal is expected");
      return NULL;
    }
    PyObject* list_obj = PyTuple_GET_ITEM(args, 0);
    ymuint n = PyList_GET_SIZE(list_obj);
    vector<Literal> assumptions(n);
    for (ymuint i = 0; i < n; ++ i) {
      PyObject* item = PyList_GET_ITEM(list_obj, i);
      if ( !PyLiteral_Check(item) ) {
	PyErr_SetString(PyExc_TypeError, "logic.Literal is expected");
	return NULL;
      }

      assumptions[i] = PyLiteral_AsLiteral(item);
    }
    stat = self->mSolver->solve(assumptions, model);
  }
  else {
    stat = self->mSolver->solve(model);
  }

  ymuint n = model.size();
  PyObject* model_obj = PyTuple_New(n);
  for (ymuint i = 0; i < n; ++ i) {
    PyObject* val_obj = PyBool3_FromBool3(model[i]);
    PyTuple_SetItem(model_obj, i, val_obj);
  }

  PyObject* stat_obj = PyBool3_FromBool3(stat);
  return Py_BuildValue("(OO)", stat_obj, model_obj);
}


// get_stats 関数
PyObject*
SatSolver_get_stats(SatSolverObject* self,
		    PyObject* args)
{
  SatStats stats;
  self->mSolver->get_stats(stats);

  // 結果を Python の dictionary に格納する．
  PyObject* dict_obj = PyDict_New();

  PyDict_SetItemString(dict_obj, "restart",            PyObject_FromYmuint64(stats.mRestart));
  PyDict_SetItemString(dict_obj, "variable_num",       PyObject_FromYmuint64(stats.mVarNum));
  PyDict_SetItemString(dict_obj, "constr_clause_num",  PyObject_FromYmuint64(stats.mConstrClauseNum));
  PyDict_SetItemString(dict_obj, "constr_literal_num", PyObject_FromYmuint64(stats.mConstrLitNum));
  PyDict_SetItemString(dict_obj, "learnt_clause_num",  PyObject_FromYmuint64(stats.mLearntClauseNum));
  PyDict_SetItemString(dict_obj, "learnt_literal_num", PyObject_FromYmuint64(stats.mLearntLitNum));
  PyDict_SetItemString(dict_obj, "conflict_num",       PyObject_FromYmuint64(stats.mConflictNum));
  PyDict_SetItemString(dict_obj, "decision_num",       PyObject_FromYmuint64(stats.mDecisionNum));
  PyDict_SetItemString(dict_obj, "propagation_num",    PyObject_FromYmuint64(stats.mPropagationNum));
  PyDict_SetItemString(dict_obj, "conflict_limit",     PyObject_FromYmuint64(stats.mConflictLimit));
  PyDict_SetItemString(dict_obj, "learnt_limit",       PyObject_FromYmuint64(stats.mLearntLimit));
  PyDict_SetItemString(dict_obj, "time",               PyUSTime_FromUSTime(stats.mTime));

  return dict_obj;
}

// variable_num 関数
PyObject*
SatSolver_variable_num(SatSolverObject* self,
		       PyObject* args)
{
  return PyObject_FromYmuint64(self->mSolver->variable_num());
}

// clause_num 関数
PyObject*
SatSolver_clause_num(SatSolverObject* self,
		     PyObject* args)
{
  return PyObject_FromYmuint64(self->mSolver->clause_num());
}

// literal_num 関数
PyObject*
SatSolver_literal_num(SatSolverObject* self,
		      PyObject* args)
{
  return PyObject_FromYmuint64(self->mSolver->literal_num());
}

// set_max_conflict 関数
PyObject*
SatSolver_set_max_conflict(SatSolverObject* self,
			   PyObject* args)
{
  ymuint64 val;
  if ( !PyArg_ParseTuple(args, "K", &val) ) {
    return NULL;
  }

  return PyObject_FromYmuint64(self->mSolver->set_max_conflict(val));
}

// reg_mesg_handler 関数
PyObject*
SatSolver_reg_msg_handler(SatSolverObject* self,
			  PyObject* args)
{
  PyObject* obj = NULL;
  if ( !PyArg_ParseTuple(args, "O", &obj) ) {
    return NULL;
  }
  if ( !PyCallable_Check(obj) ) {
    PyErr_SetString(PyExc_TypeError, "parameter must be callable");
    return NULL;
  }

  PySatMsgHandler* handler = new PySatMsgHandler(obj);
  if ( handler == NULL ) {
    return NULL;
  }

  self->mSolver->reg_msg_handler(handler);

  Py_INCREF(Py_None);
  return Py_None;
}

// timer_on 関数
PyObject*
SatSolver_timer_on(SatSolverObject* self,
		   PyObject* args)
{
  bool enable;
  if ( !PyArg_ParseTuple(args, "b", &enable) ) {
    return NULL;
  }

  self->mSolver->timer_on(enable);

  Py_INCREF(Py_None);
  return Py_None;
}


//  SatSolver のメソッドテーブル
PyMethodDef SatSolver_methods[] = {
  {"sane", (PyCFunction)SatSolver_sane, METH_NOARGS,
   PyDoc_STR("return true if solver is sane (NONE)")},
  {"new_var", (PyCFunction)SatSolver_new_var, METH_NOARGS,
   PyDoc_STR("add new variable. return VarId")},
  {"add_clause", (PyCFunction)SatSolver_add_clause, METH_VARARGS,
   PyDoc_STR("add new clause. (list of Literal)")},
  {"solve", (PyCFunction)SatSolver_solve, METH_VARARGS,
   PyDoc_STR("solve the SAT problem (?list of Literal)")},
  {"get_stats", (PyCFunction)SatSolver_get_stats, METH_NOARGS,
   PyDoc_STR("return current statistics (NONE)")},
  {"variable_num", (PyCFunction)SatSolver_variable_num, METH_NOARGS,
   PyDoc_STR("return the number of variables. (NONE)")},
  {"clause_num", (PyCFunction)SatSolver_clause_num, METH_NOARGS,
   PyDoc_STR("return the number of clauses. (NONE)")},
  {"literal_num", (PyCFunction)SatSolver_literal_num, METH_NOARGS,
   PyDoc_STR("return the number of literals. (NONE)")},
  {"set_max_conflict", (PyCFunction)SatSolver_set_max_conflict, METH_VARARGS,
   PyDoc_STR("set the maximum conflict limit. (ulonglong)")},
  {"reg_msg_handler", (PyCFunction)SatSolver_reg_msg_handler, METH_VARARGS,
   PyDoc_STR("register message handler callback (MsgHandler)")},
  {"timer_on", (PyCFunction)SatSolver_timer_on, METH_VARARGS,
   PyDoc_STR("enable timer (Bool)")},
  {NULL, NULL, 0, NULL}
};

END_NONAMESPACE


// SatSolverObject 用のタイプオブジェクト
PyTypeObject PySatSolver_Type = {
  /* The ob_type field must be initialized in the module init function
   * to be portable to Windows without using C++. */
  PyVarObject_HEAD_INIT(NULL, 0)
  "logic.SatSolver",            /*tp_name*/
  sizeof(SatSolverObject),      /*tp_basicsize*/
  0,                          /*tp_itemsize*/
  /* methods */
  (destructor)SatSolver_dealloc,    /*tp_dealloc*/
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
  "SAT Solver",               /*tp_doc*/
  0,                          /*tp_traverse*/
  0,                          /*tp_clear*/
  0,                          /*tp_richcompare*/
  0,                          /*tp_weaklistoffset*/
  0,                          /*tp_iter*/
  0,                          /*tp_iternext*/
  SatSolver_methods,             /*tp_methods*/
  0,                          /*tp_members*/
  0,                          /*tp_getset*/
  0,                          /*tp_base*/
  0,                          /*tp_dict*/
  0,                          /*tp_descr_get*/
  0,                          /*tp_descr_set*/
  0,                          /*tp_dictoffset*/
  (initproc)SatSolver_init,      /*tp_init*/
  0,                          /*tp_alloc*/
  (newfunc)SatSolver_new,        /*tp_new*/
  0,                          /*tp_free*/
  0,                          /*tp_is_gc*/
};

// @brief PyObject から SatSolver へのポインタを取り出す．
// @param[in] py_obj Python オブジェクト
// @return SatSolver へのポインタを返す．
// @note 変換が失敗したら TypeError を送出し，NULL を返す．
SatSolver*
PySatSolver_AsSatSolverPtr(PyObject* py_obj)
{
  // 型のチェック
  if ( !PySatSolver_Check(py_obj) ) {
    PyErr_SetString(PyExc_TypeError, "logic.SatSolver is expected");
    return NULL;
  }

  // 強制的にキャスト
  SatSolverObject* satsolver_obj = (SatSolverObject*)py_obj;

  return satsolver_obj->mSolver;
}

// SatSolverObject 関係の初期化を行う．
void
SatSolverObject_init(PyObject* m)
{
  // タイプオブジェクトの初期化
  if ( PyType_Ready(&PySatSolver_Type) < 0 ) {
    return;
  }

  // タイプオブジェクトの登録
  PyModule_AddObject(m, "SatSolver", (PyObject*)&PySatSolver_Type);
}

END_NAMESPACE_YM
