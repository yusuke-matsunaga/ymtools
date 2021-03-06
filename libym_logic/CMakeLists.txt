# ===================================================================
# libym_logic のための CMakeLists.txt
# ===================================================================


# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  include
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory(gtest)


# ===================================================================
#  ソースファイルの設定
# ===================================================================
set (aig_SOURCES
  src/aig/AigMgr.cc
  src/aig/AigMgrImpl.cc
  src/aig/AigNode.cc
  )

set (bdd_SOURCES
  src/bdd/base/AeOp.cc
  src/bdd/base/AndOp.cc
  src/bdd/base/Bdd.cc
  src/bdd/base/BddBinOp.cc
  src/bdd/base/BddList.cc
  src/bdd/base/BddLitSet.cc
  src/bdd/base/BddMgr.cc
  src/bdd/base/BddMgrImpl.cc
  src/bdd/base/BddTriOp.cc
  src/bdd/base/BddUniOp.cc
  src/bdd/base/BddVarSet.cc
  src/bdd/base/BddVector.cc
  src/bdd/base/CofOp.cc
  src/bdd/base/CompTbl.cc
  src/bdd/base/CompTbl1.cc
  src/bdd/base/CompTbl2.cc
  src/bdd/base/CompTbl3.cc
  src/bdd/base/CountOp.cc
  src/bdd/base/Dumper.cc
  src/bdd/base/GcOp.cc
  src/bdd/base/IdMgr.cc
  src/bdd/base/IntsecOp.cc
  src/bdd/base/IteOp.cc
  src/bdd/base/McOp.cc
  src/bdd/base/Printer.cc
  src/bdd/base/Restorer.cc
  src/bdd/base/SmoothOp.cc
  src/bdd/base/SupOp.cc
  src/bdd/base/SymOp.cc
  src/bdd/base/W0Op.cc
  src/bdd/base/W1Op.cc
  src/bdd/base/XcOp.cc
  src/bdd/base/XorOp.cc

  src/bdd/bmc/BddMgrClassic.cc
  src/bdd/bmc/BmcCompTbl.cc
  src/bdd/bmc/bmc_isop.cc
  src/bdd/bmc/bmc_ite.cc
  src/bdd/bmc/bmc_ls.cc
  src/bdd/bmc/bmc_onepath.cc
  src/bdd/bmc/bmc_vs.cc

  src/bdd/bmm/BddMgrModern.cc
  src/bdd/bmm/BmmCompTbl.cc
  src/bdd/bmm/BmmVar.cc
  src/bdd/bmm/bmm_isop.cc
  src/bdd/bmm/bmm_ite.cc
  src/bdd/bmm/bmm_ls.cc
  src/bdd/bmm/bmm_onepath.cc
  src/bdd/bmm/bmm_vs.cc
  )

set (cnfbdd_SOURCES
  src/cnfdd/BinOp.cc
  src/cnfdd/CNFdd.cc
  src/cnfdd/CNFddMgr.cc
  src/cnfdd/CNFddMgrImpl.cc
  src/cnfdd/Cof0Op.cc
  src/cnfdd/CofNOp.cc
  src/cnfdd/CofPOp.cc
  src/cnfdd/CompTbl.cc
  src/cnfdd/ConOp.cc
  src/cnfdd/CutOp.cc
  src/cnfdd/DiffOp.cc
  src/cnfdd/DiffOp2.cc
  src/cnfdd/DisOp.cc
  src/cnfdd/Dumper.cc
  src/cnfdd/IdMgr.cc
  src/cnfdd/LitNOp.cc
  src/cnfdd/LitPOp.cc
  src/cnfdd/MergeOp.cc
  src/cnfdd/MinOp.cc
  src/cnfdd/Printer.cc
  src/cnfdd/Printer2.cc
  src/cnfdd/Restorer.cc
  src/cnfdd/SupOp.cc
  src/cnfdd/UniNOp.cc
  src/cnfdd/UniOp.cc
  src/cnfdd/UniVOp.cc
  src/cnfdd/count.cc
  )

set (expr_SOURCES
  src/expr/Expr.cc
  src/expr/ExprMgr.cc
  src/expr/ExprNode.cc
  src/expr/ExprParser.cc
  src/expr/ExprWriter.cc
  src/expr/Literal.cc
  )

set (npn_SOURCES
  src/npn/NpnBaseConf.cc
  src/npn/NpnConf.cc
  src/npn/NpnMap.cc
  src/npn/NpnMapM.cc
  src/npn/NpnMgr.cc
  src/npn/NpnMgrImpl.cc
  src/npn/NpnRawSig.cc
  )

set (sat_SOURCES
  src/sat/SatMsgHandlerImpl1.cc
  src/sat/SatSolver.cc
  src/sat/SatStats.cc

  src/sat/MiniSat/SatSolverMiniSat.cc
  src/sat/MiniSat/Solver.C

  src/sat/MiniSat2/Options.cc
  src/sat/MiniSat2/SatSolverMiniSat2.cc
  src/sat/MiniSat2/Solver.cc
  src/sat/MiniSat2/System.cc

  src/sat/glueminisat-2.2.8/SatSolverGlueMiniSat2.cc
  src/sat/glueminisat-2.2.8/Options.cc
  src/sat/glueminisat-2.2.8/Solver.cc
  src/sat/glueminisat-2.2.8/System.cc

  src/sat/dimacs/DimacsParser.cc
  src/sat/dimacs/DimacsScanner.cc
  src/sat/dimacs/DimacsVerifier.cc
  src/sat/dimacs/SatDimacsHandler.cc

  src/sat/ymsat/AssignList.cc
  src/sat/ymsat/SaBase.cc
  src/sat/ymsat/SaFactory.cc
  src/sat/ymsat/SaUIP1.cc
  src/sat/ymsat/SaUIP2.cc
  src/sat/ymsat/SatClause.cc
  src/sat/ymsat/VarHeap.cc
  src/sat/ymsat/YmSat_base.cc
  src/sat/ymsat/YmSat_solve.cc
  src/sat/ymsat/YmSatMS2.cc
  )

set (tvfunc_SOURCES
  src/tvfunc/TvFunc.cc
  src/tvfunc/TvFuncM.cc
  )

set (zdd_SOURCES
  src/zdd/CapOp.cc
  src/zdd/Cof0Op.cc
  src/zdd/Cof1Op.cc
  src/zdd/CompTbl.cc
  src/zdd/CupOp.cc
  src/zdd/DiffOp.cc
  src/zdd/Dumper.cc
  src/zdd/IdMgr.cc
  src/zdd/MergeOp.cc
  src/zdd/MergeOp2.cc
  src/zdd/Ms2Op.cc
  src/zdd/MsOp.cc
  src/zdd/NeOp.cc
  src/zdd/Printer.cc
  src/zdd/Printer2.cc
  src/zdd/Restorer.cc
  src/zdd/SupOp.cc
  src/zdd/SwapOp.cc
  src/zdd/Zdd.cc
  src/zdd/ZddBinOp.cc
  src/zdd/ZddList.cc
  src/zdd/ZddMgr.cc
  src/zdd/ZddMgrImpl.cc
  src/zdd/ZddUniOp.cc
  src/zdd/ZddVector.cc
  src/zdd/count.cc
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================

ym_add_library(ym_logic
  SHARED
  ${aig_SOURCES}
  ${bdd_SOURCES}
  ${cnfdd_SOURCES}
  ${expr_SOURCES}
  ${npn_SOURCES}
  ${sat_SOURCES}
  ${tvfunc_SOURCES}
  ${zdd_SOURCES}
  )

target_link_libraries(ym_logic
  ym_utils
  )

target_link_libraries(ym_logic_p
  ym_utils_p
  )

target_link_libraries(ym_logic_d
  ym_utils_d
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================

add_executable(ymsat
  tests/sat/ymsat.cc
  )

target_link_libraries(ymsat
  ym_logic
  )


# ===================================================================
#  インストールターゲットの設定
# ===================================================================

install(
  TARGETS ym_logic ym_logic_p ym_logic_d
  DESTINATION lib
  EXPORT ymtools-targets
  )

install(
  DIRECTORY YmLogic
  DESTINATION include
  )
