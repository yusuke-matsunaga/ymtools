
/// @file bb.cc
/// @brief bb のメイン関数
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "Parser.h"
#include "PtMgr.h"
#include "Var.h"
#include "Assign.h"
#include "Constr.h"
#include "PtNode.h"

#include "ym_logic/AigMgr.h"
//#include "ym_logic/AigSatMgr.h"
#include "ym_logic/SatSolver.h"
#include "ym_utils/StopWatch.h"
#include "ym_utils/MsgHandler.h"


BEGIN_NAMESPACE_YM_BB

void
bb(const char* file_name,
   ymuint loop_num)
{
  MsgMgr msgmgr;
  PtMgr ptmgr;

  StreamMsgHandler* msg_handler = new StreamMsgHandler(&cerr);
  msgmgr.reg_handler(msg_handler);

  Parser parser(msgmgr, ptmgr);

  cout << "reading " << file_name << endl;
  if ( !parser.read(file_name) ) {
    cout << "  failed" << endl;
    return;
  }
  cout << "  succeed" << endl;

  const vector<Var*>& var_list = ptmgr.var_list();
  const vector<Assign*>& assign_list = ptmgr.assign_list();
  const vector<Constr*>& constr_list = ptmgr.constr_list();

  for (vector<Var*>::const_iterator p = var_list.begin();
       p != var_list.end(); ++ p) {
    Var* var = *p;
    cout << "Var#" << var->id() << ": ";
    if ( var->delta() > 0 ) {
      cout << var->start()
	   << ", " << var->end()
	   << ", " << var->delta();
    }
    cout << endl;
  }

  for (vector<Assign*>::const_iterator p = assign_list.begin();
       p != assign_list.end(); ++ p) {
    Assign* assign = *p;
    cout << "Var#" << assign->lhs_id()
	 << " = " << assign->rhs()->decompile() << endl;
  }

  for (vector<Constr*>::const_iterator p = constr_list.begin();
       p != constr_list.end(); ++ p) {
    Constr* constr = *p;
    const char* type_str = NULL;
    switch ( constr->type() ) {
    case Constr::kEq: type_str = "EQ"; break;
    case Constr::kLt: type_str = "LT"; break;
    case Constr::kLe: type_str = "LE"; break;
    case Constr::kGt: type_str = "GT"; break;
    case Constr::kGe: type_str = "GE"; break;
    }
    cout << "Type: " << type_str << endl
	 << "Lhs: " << constr->lhs()->decompile() << endl
	 << "Rhs: " << constr->rhs()->decompile() << endl;
  }

  ymuint bw = parser.bit_width();

  StopWatch timer;
  timer.start();
  for (ymuint c = 0; c < loop_num; ++ c) {
    AigMgr aigmgr;
    ymuint nv = var_list.size();
    vector<Aig> bvar_array(nv * bw);
    for (ymuint i = 0; i < nv; ++ i) {
      for (ymuint j = 0; j < bw; ++ j) {
	ymuint idx = i * bw + j;
	bvar_array[idx] = aigmgr.make_input(VarId(idx));
      }
    }
    vector<Aig> root_list;
    root_list.reserve(constr_list.size() * bw);
    for (vector<Constr*>::const_iterator p = constr_list.begin();
	 p != constr_list.end(); ++ p) {
      Constr* constr = *p;
      constr->gen_aig(aigmgr, bvar_array, bw, root_list);
    }

    bool have_zero = false;
    bool all_one = true;
    for (vector<Aig>::iterator p = root_list.begin();
	 p != root_list.end(); ++ p) {
      Aig root = *p;
      if ( root.is_zero() ) {
	have_zero = true;
	break;
      }
      if ( !root.is_one() ) {
	all_one = false;
      }
    }

    if ( c == loop_num - 1 ) {
      cout << "# of inputs: " << aigmgr.input_num() << endl;
#if 0
      cout << "root" << endl;
      aigmgr.dump_handles(cout, root_list);
#endif
      cout << "# of AIG nodes: " << aigmgr.node_num() << endl;
      if ( have_zero ) {
	cout << "Never conflict(1)" << endl;
      }
      else if ( all_one ) {
	cout << "May conflict(1)" << endl;
      }
    }

    if ( !have_zero && !all_one ) {
#if 0
      SatSolver solver;
      for (vector<Aig>::iterator p = root_list.begin();
	   p != root_list.end(); ++ p) {
	Aig root = *p;
	assert_cond( !root.is_zero(), __FILE__, __LINE__);
	if ( root.is_one() ) {
	  continue;
	}
      }
      AigSatMgr aigsat(aigmgr, solver);
      vector<Bool3> model;
      Bool3 stat = aigsat.sat(root_list, model);
      if ( c == loop_num - 1 ) {
	SatStats stats;
	solver.get_stats(stats);
	cout << "# of Variables: " << stats.mVarNum << endl
	     << "# of Constr Clauses: " << stats.mConstrClauseNum << endl
	     << "# of Learnt Clauses: " << stats.mLearntClauseNum << endl
	     << endl;
	if ( stat == kB3True ) {
	  cout << "May conflict(2)" << endl;
	}
	else if ( stat == kB3False ) {
	  cout << "Never conflict(2)" << endl;
	}
	else {
	  cout << "Unknown." << endl;
	}
      }
#else
      if ( c == loop_num - 1 ) {
	cout << "Unknown" << endl;
      }
#endif
    }
  }
  timer.stop();
  USTime time = timer.time();
  cout << "Total " << time << " for " << loop_num
       << " iterations" << endl;
}

END_NAMESPACE_YM_BB


int
main(int argc,
     char** argv)
{
  using namespace std;

  if ( argc != 2 ) {
    cerr << "USAGE: " << argv[0] << " file-name" << endl;
    return 1;
  }

  nsYm::nsBb::bb(argv[1], 1000);

  return 0;
}
