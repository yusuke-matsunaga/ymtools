
/// @file MinCov.sip
/// @brief MinCov の sip ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.

class MinCov
{
%TypeHeaderCode
#include "utils/MinCov.h"
using nsYm::MinCov;
%End

public:

  /// @brief コンストラクタ
  MinCov();

  /// @brief 問題のサイズを設定する．
  /// @param[in] row_size 行数
  /// @param[in] col_size 列数
  void
  set_size(unsigned int row_size,
	   unsigned int col_size);

  /// @brief 列のコストを設定する
  /// @param[in] col_pos 追加する要素の列番号
  /// @param[in] cost コスト
  void
  set_col_cost(unsigned int col_pos,
	       double cost);

  /// @brief 要素を追加する．
  /// @param[in] row_pos 追加する要素の行番号
  /// @param[in] col_pos 追加する要素の列番号
  void
  insert_elem(unsigned int row_pos,
	      unsigned int col_pos);

  /// @brief 最小被覆問題を解く．
  /// @param[out] solution 選ばれた列集合
  /// @return 解のコスト
  SIP_PYTUPLE
  exact();
  %MethodCode
  std::vector<ymuint32> solution;
  ymuint cost = sipCpp->exact(solution);
  ymuint n = solution.size();
  PyObject* py_list = PyList_New(n);
  for (ymuint i = 0; i < n; ++ i) {
    PyObject* obj = sipBuildResult(NULL, "i", solution[i]);
    Py_INCREF(obj);
    PyList_SetItem(py_list, i, obj);
  }
  sipRes = sipBuildResult(NULL, "(iR)", cost, py_list);
  %End

  /// @brief ヒューリスティックで最小被覆問題を解く．
  /// @param[out] solution 選ばれた列集合
  /// @return 解のコスト
  SIP_PYTUPLE
  heuristic();
  %MethodCode
  std::vector<ymuint32> solution;
  ymuint cost = sipCpp->heuristic(solution);
  ymuint n = solution.size();
  PyObject* py_list = PyList_New(n);
  for (ymuint i = 0; i < n; ++ i) {
    PyObject* obj = sipBuildResult(NULL, "i", solution[i]);
    Py_INCREF(obj);
    PyList_SetItem(py_list, i, obj);
  }
  sipRes = sipBuildResult(NULL, "(iR)", cost, py_list);
  %End

};
