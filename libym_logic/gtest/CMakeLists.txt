# ===================================================================
# libym_utils/gtest/CMakeLists.txt
# ===================================================================


# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${GTEST_INCLUDE_DIR}
  )


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set (misc_SOURCES
  misc/VarIdTest.cc
  misc/LiteralTest.cc
  misc/Bool3Test.cc
  )

set (sat_SOURCES
  )


# ===================================================================
#  テストターゲットの設定
# ===================================================================

add_executable(YmLogicTest
  ${misc_SOURCES}
  ${sat_SOURCES}
  )

target_compile_options (YmLogicTest
  PRIVATE "-g"
  )

target_link_libraries(YmLogicTest
  pthread
  ym_logic_d
  ym_utils_d
  ${GTEST_BOTH_LIBRARIES}
  )

add_test(AllTestsInYmLogic
  YmLogicTest
  )
