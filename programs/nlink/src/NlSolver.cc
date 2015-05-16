
/// @file NlSolver.cc
/// @brief NlSolver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "NlSolver.h"
#include "MazeRouter.h"
#include "YmLogic/SatSolver.h"


BEGIN_NAMESPACE_YM_NLINK

//////////////////////////////////////////////////////////////////////
// クラス NlSolver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
NlSolver::NlSolver()
{
}

// @brief デストラクタ
NlSolver::~NlSolver()
{
}

BEGIN_NONAMESPACE

inline
void
one_hot2(SatSolver& solver,
	 Literal lit1,
	 Literal lit2)
{
  solver.add_clause(~lit1, ~lit2);
  solver.add_clause( lit1,  lit2);
}

inline
void
one_hot3(SatSolver& solver,
	 Literal lit1,
	 Literal lit2,
	 Literal lit3)
{
  solver.add_clause(~lit1, ~lit2);
  solver.add_clause(~lit1, ~lit3);
  solver.add_clause(~lit2, ~lit3);
  solver.add_clause( lit1,  lit2,  lit3);
}

inline
void
one_hot4(SatSolver& solver,
	 Literal lit1,
	 Literal lit2,
	 Literal lit3,
	 Literal lit4)
{
  solver.add_clause(~lit1, ~lit2);
  solver.add_clause(~lit1, ~lit3);
  solver.add_clause(~lit1, ~lit4);
  solver.add_clause(~lit2, ~lit3);
  solver.add_clause(~lit2, ~lit4);
  solver.add_clause(~lit3, ~lit4);
  solver.add_clause( lit1,  lit2,  lit3,  lit4);
}

inline
void
zero_two_hot2(SatSolver& solver,
	      Literal lit1,
	      Literal lit2)
{
  solver.add_clause(~lit1,  lit2);
  solver.add_clause( lit1, ~lit2);
}

inline
void
zero_two_hot3(SatSolver& solver,
	      Literal lit1,
	      Literal lit2,
	      Literal lit3)
{
  solver.add_clause(~lit1, ~lit2, ~lit3);
  solver.add_clause( lit1, ~lit2,  lit3);
  solver.add_clause( lit1,  lit2, ~lit3);
  solver.add_clause(~lit1,  lit2,  lit3);
}

inline
void
zero_two_hot4(SatSolver& solver,
	      Literal lit1,
	      Literal lit2,
	      Literal lit3,
	      Literal lit4)
{
  solver.add_clause( lit1,  lit2,  lit3, ~lit4);
  solver.add_clause( lit1,  lit2, ~lit3,  lit4);
  solver.add_clause( lit1, ~lit2,  lit3,  lit4);
  solver.add_clause( lit1, ~lit2, ~lit3, ~lit4);
  solver.add_clause(~lit1,  lit2,  lit3,  lit4);
  solver.add_clause(~lit1,  lit2, ~lit3, ~lit4);
  solver.add_clause(~lit1, ~lit2,  lit3, ~lit4);
  solver.add_clause(~lit1, ~lit2, ~lit3,  lit4);
  solver.add_clause(~lit1, ~lit2, ~lit3, ~lit4);
}

void
print_solution(const NlProblem& problem,
	       const vector<Bool3>& model,
	       const vector<Literal>& node_array,
	       const vector<Literal>& h_array,
	       const vector<Literal>& v_array)
{
  ymuint w = problem.width();
  ymuint h = problem.height();
  ymuint n = problem.elem_num();

  for (ymuint y = 0; y < h; ++ y) {
    for (ymuint x = 0; x < w; ++ x) {
      ymuint base = ((x * h) + y) * n;
      bool fill = false;
      for (ymuint k = 0; k < n; ++ k) {
	Literal lit = node_array[base + k];
	VarId var = lit.varid();
	if ( model[var.val()] == kB3True ) {
	  cout << " " << setw(2) << (k + 1);
	  fill = true;
	  break;
	}
      }
      if ( !fill ) {
	cout << "   ";
      }
    }
    cout << endl;
  }
}

END_NONAMESPACE

// @brief 問題を解く
void
NlSolver::solve(const NlProblem& problem)
{
  SatSolver solver("minisat2", string(), NULL);

  ymuint w = problem.width();
  ymuint h = problem.height();
  ymuint n = problem.elem_num();

  ASSERT_COND( w > 0 );
  ASSERT_COND( h > 0 );
  ASSERT_COND( n > 0 );

  vector<ymuint> end_mark(w * h, 0);
  vector<Literal> node_array(w * h * n);
  vector<Literal> h_array((w - 1) * h);
  vector<Literal> v_array(w * (h - 1));

  // ノードの変数
  // 各位置に対して n 個用意する．
  for (ymuint x = 0; x < w; ++ x) {
    for (ymuint y = 0; y < h; ++ y) {
      for (ymuint k = 0; k < n; ++ k) {
	VarId var = solver.new_var();
	node_array[(x * h + y) * n + k] = Literal(var);
      }
    }
  }

  // 横方向の枝の変数
  for (ymuint x = 1; x < w; ++ x) {
    for (ymuint y = 0; y < h; ++ y) {
      VarId var = solver.new_var();
      h_array[(x - 1) * h + y] = Literal(var);
    }
  }

  // 縦方向の枝の変数
  for (ymuint y = 1; y < h; ++ y) {
    for (ymuint x = 0; x < w; ++ x) {
      VarId var = solver.new_var();
      v_array[(y - 1) * w + x] = Literal(var);
    }
  }

  // 始点と終点に印をつける．
  for (ymuint k = 0; k < n; ++ k) {
    NlConnection con = problem.connection(k);
    {
      NlPoint start_point = con.start_point();
      ymuint x1 = start_point.x();
      ymuint y1 = start_point.y();
      Literal lit1 = node_array[(x1 * h + y1) * n + k];
      solver.add_clause(lit1);
      end_mark[x1 * h + y1] = k + 1;
    }
    {
      NlPoint end_point = con.end_point();
      ymuint x2 = end_point.x();
      ymuint y2 = end_point.y();
      Literal lit2 = node_array[(x2 * h + y2) * n + k];
      solver.add_clause(lit2);
      end_mark[x2 * h + y2] = k + 1;
    }
  }

  // one-hot 制約を作る．
  // 一つのノードでは高々1つの変数しか1にならない．
  for (ymuint x = 0; x < w; ++ x) {
    for (ymuint y = 0; y < h; ++ y) {
      ymuint base = (x * h + y) * n;
      for (ymuint k1 = 0; k1 < n; ++ k1) {
	Literal lit1 = node_array[base + k1];
	for (ymuint k2 = k1 + 1; k2 < n; ++ k2) {
	  Literal lit2 = node_array[base + k2];
	  solver.add_clause(~lit1, ~lit2);
	}
      }
    }
  }

  // 枝の条件を作る．
  for (ymuint x = 0; x < w; ++ x) {
    for (ymuint y = 0; y < h; ++ y) {
      // (x, y) のノードに隣接する枝のリストを作る．
      vector<Literal> edge_list;
      if ( x > 0 ) {
	// 左の枝を加える．
	Literal lit = h_array[(x - 1) * h + y];
	edge_list.push_back(lit);
      }
      if ( x < w - 1 ) {
	// 右の枝を加える．
	Literal lit = h_array[x * h + y];
	edge_list.push_back(lit);
      }
      if ( y > 0 ) {
	// 上の枝を加える．
	Literal lit = v_array[(y - 1) * w + x];
	edge_list.push_back(lit);
      }
      if ( y < h - 1 ) {
	// 下の枝を加える．
	Literal lit = v_array[y * w + x];
	edge_list.push_back(lit);
      }

      if ( end_mark[x * h + y] > 0 ) {
	// 端点の場合
	// 必ずただ1つの枝が選ばれる．
	switch ( edge_list.size() ) {
	case 2:
	  one_hot2(solver, edge_list[0], edge_list[1]);
	  break;

	case 3:
	  one_hot3(solver, edge_list[0], edge_list[1], edge_list[2]);
	  break;

	case 4:
	  one_hot4(solver, edge_list[0], edge_list[1], edge_list[2], edge_list[3]);
	  break;

	default:
	  ASSERT_NOT_REACHED;
	}
      }
      else {
	// そうでない場合
	// 0個か2個の枝が選ばれる．
	switch ( edge_list.size() ) {
	case 2:
	  zero_two_hot2(solver, edge_list[0], edge_list[1]);
	  break;

	case 3:
	  zero_two_hot3(solver, edge_list[0], edge_list[1], edge_list[2]);
	  break;

	case 4:
	  zero_two_hot4(solver, edge_list[0], edge_list[1], edge_list[2], edge_list[3]);
	  break;

	default:
	  ASSERT_NOT_REACHED;
	}
      }
    }
  }

  // 横方向の隣接関係の条件を作る．
  for (ymuint x2 = 1; x2 < w; ++ x2) {
    ymuint x1 = x2 - 1;
    for (ymuint y = 0; y < h; ++ y) {
      Literal e_lit = h_array[x1 * h + y];
      ymuint base1 = ((x1 * h) + y) * n;
      ymuint base2 = ((x2 * h) + y) * n;
      for (ymuint k = 0; k < n; ++ k) {
	Literal v_lit1 = node_array[base1 + k];
	Literal v_lit2 = node_array[base2 + k];
	solver.add_clause(~e_lit,  v_lit1, ~v_lit2);
	solver.add_clause(~e_lit, ~v_lit1,  v_lit2);
      }
    }
  }

  // 縦方向の隣接関係の条件を作る．
  for (ymuint y2 = 1; y2 < h; ++ y2) {
    ymuint y1 = y2 - 1;
    for (ymuint x = 0; x < w; ++ x) {
      Literal e_lit = v_array[y1 * w + x];
      ymuint base1 = ((x * h) + y1) * n;
      ymuint base2 = ((x * h) + y2) * n;
      for (ymuint k = 0; k < n; ++ k) {
	Literal v_lit1 = node_array[base1 + k];
	Literal v_lit2 = node_array[base2 + k];
	solver.add_clause(~e_lit,  v_lit1, ~v_lit2);
	solver.add_clause(~e_lit, ~v_lit1,  v_lit2);
      }
    }
  }

#if 0
  MazeRouter mr(problem);

  // セパレータの制約を作る．
  for (ymuint i = 0; i < n; ++ i) {
    cout << "LINE#" << (i + 1) << endl;
    ymuint lmax = mr.labeling(i);
    for (ymuint l = 1; l < lmax; ++ l) {
      vector<NlPoint> point_list;
      mr.get_cell_list(l, point_list);
      cout << "  @" << l << " ";
      for (ymuint j = 0; j < point_list.size(); ++ j) {
	const NlPoint& point = point_list[j];
	cout << point;
      }
      cout << endl;
      vector<Literal> tmp_lits;
      tmp_lits.reserve(point_list.size());
      for (ymuint j = 0; j < point_list.size(); ++ j) {
	const NlPoint& point = point_list[j];
	ymuint index = ((point.x() * h) + point.y()) * n + i;
	Literal lit = node_array[index];
	tmp_lits.push_back(lit);
      }
      solver.add_clause(tmp_lits);
    }
  }
#endif

  vector<Bool3> model;
  Bool3 stat = solver.solve(model);
  switch ( stat ) {
  case kB3True:
    cout << "SAT" << endl;
    print_solution(problem, model, node_array, h_array, v_array);
    break;

  case kB3False:
    cout << "UNSAT" << endl;
    break;

  case kB3X:
    cout << "ABORT" << endl;
    break;
  }
}

END_NAMESPACE_YM_NLINK
