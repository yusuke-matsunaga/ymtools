# ===================================================================
# libym_algo/gtest/CMakeLists.txt
# ===================================================================

enable_testing()


# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${GTEST_INCLUDE_DIR}
  )


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set ( btg_SOURCES
  btg/BtgMatchTest.cc
  )


# ===================================================================
#  テストターゲットの設定
# ===================================================================

add_executable(YmAlgoTest
  ${btg_SOURCES}
  )

target_compile_options (YmAlgoTest
  PRIVATE "-g"
  )

target_link_libraries(YmAlgoTest
  pthread
  ym_algo_d
  ym_utils_d
  ${GTEST_BOTH_LIBRARIES}
  )

add_test(AllTestsInYmAlgo
  YmAlgoTest
  )
