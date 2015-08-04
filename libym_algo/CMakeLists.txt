# ===================================================================
# libym_algo のための CMakeLists.txt
# ===================================================================


# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory(gtest)


# ===================================================================
#  ソースファイルの設定
# ===================================================================
set (btg_SOURCES
  src/btg/BtgHeapTree.cc
  src/btg/BtgMatch.cc
  src/btg/BtgMatchImpl.cc
  )

set (graph_SOURCES
  src/graph/GcSolver.cc
  src/graph/Graph.cc
  src/graph/independent_set.cc
  src/graph/max_clique.cc
  src/graph/max_clique_exact.cc
  )

set (mincov_SOURCES
  src/mincov/LbCS.cc
  src/mincov/LbMAX.cc
  src/mincov/LbMIS1.cc
  src/mincov/LbMIS2.cc
  src/mincov/LbMIS3.cc
  src/mincov/MaxClique.cc
  src/mincov/McMatrix.cc
  src/mincov/McSolver.cc
  src/mincov/McSolverImpl.cc
  src/mincov/MinCov.cc
  src/mincov/MisNodeHeap.cc
  src/mincov/SelCS.cc
  src/mincov/SelNaive.cc
  src/mincov/SelSimple.cc
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================

ym_add_library(ym_algo
  SHARED
  ${btg_SOURCES}
  ${graph_SOURCES}
  ${mincov_SOURCES}
  )


# ===================================================================
#  インストールターゲットの設定
# ===================================================================

install(
  TARGETS ym_algo ym_algo_p ym_algo_d
  DESTINATION lib
  EXPORT ymtools-targets
  )

install(
  DIRECTORY YmAlgo
  DESTINATION include
  )
