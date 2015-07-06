# ===================================================================
# libym_utils/gtest/CMakeLists.txt
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

set (alloc_SOURCES
  alloc/SimpleAllocTest.cc
  alloc/UnitAllocTest.cc
  alloc/FragAllocTest.cc
  )

set (gen_SOURCES
  gen/CombiGenTest.cc
  gen/PermGenTest.cc
  )


# ===================================================================
#  テストターゲットの設定
# ===================================================================

add_executable(YmUtilsTest
  ${alloc_SOURCES}
  ${gen_SOURCES}
  )

target_link_libraries(YmUtilsTest
  pthread
  ym_utils
  ${GTEST_BOTH_LIBRARIES}
  )

add_test(AllTestsInYmUtils
  YmUtilsTest
  )
