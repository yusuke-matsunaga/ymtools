
/// @file pycell_init.cc
/// @brief libym_cell の Python 用拡張モジュールの初期化
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012 Yusuke Matsunaga
/// All rights reserved.


//////////////////////////////////////////////////////////////////////
// pycell モジュール
//////////////////////////////////////////////////////////////////////

#include "ym_cell/pycell.h"


BEGIN_NAMESPACE_YM_PYTHON

BEGIN_NONAMESPACE

// メソッドテーブル
PyMethodDef cell_methods[] = {
  {NULL, NULL}
};

END_NONAMESPACE


void
CellAreaObject_init(PyObject* m);

void
CellCapacitanceObject_init(PyObject* m);

void
CellTimeObject_init(PyObject* m);

void
CellLibraryObject_init(PyObject* m);

END_NAMESPACE_YM_PYTHON


PyMODINIT_FUNC
cell_init()
{
  using namespace nsYm::nsPython;

  // モジュールオブジェクトの生成
  PyDoc_STRVAR(module_doc,
	       "Module for libym_cell");
  PyObject* m = Py_InitModule3("cell", cell_methods, module_doc);
  if ( m == NULL ) {
    return;
  }

  // CellAreaObject 関係の初期化
  CellAreaObject_init(m);

  // CellCapacitanceObject 関係の初期化
  CellCapacitanceObject_init(m);

  // CellTimeObject 関係の初期化
  CellTimeObject_init(m);

  // CellLibraryObject 関係の初期化
  CellLibraryObject_init(m);

}