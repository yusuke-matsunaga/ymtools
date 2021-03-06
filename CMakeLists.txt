# ===================================================================
# CMAKE のおまじない
# ===================================================================
cmake_minimum_required (VERSION 3.2)


# ===================================================================
# プロジェクト名，バージョンの設定
# ===================================================================
project (YmTools)
set (YMTOOLS_VERSION_MAJOR 1)
set (YMTOOLS_VERSION_MINOR 0)
set (YMTOOLS_VERSION "${YMTOOLS_VERSION_MAJOR}.${YMTOOLS_VERSION_MINOR}")

# 名前空間
set (YMTOOLS_NAMESPACE "nsYm")

# C++11 の指定
set (CMAKE_CXX_STANDARD 11)


# ===================================================================
# オプション
# ===================================================================


# ===================================================================
# パッケージの検査
# ===================================================================
list ( APPEND CMAKE_MODULE_PATH
  "${PROJECT_SOURCE_DIR}/ym-common/cmake"
  )

include (FindDoxygen)

find_package(BISON REQUIRED)

find_package(Gperftools)

if ( UNIX )
  set ( YM_UNIX 1 )
endif ( UNIX )
if ( APPLE )
  set ( YM_APPLE 1 )
endif ( APPLE )
if ( WIN32 )
  set ( YM_WIN32 1 )
endif ( WIN32 )

include (ym_add_library)


# ===================================================================
# CTest モジュールの読み込み
# ===================================================================
include (CTest)

enable_testing()


# ===================================================================
# google-test は内蔵のものを使う．
# ===================================================================
include ( ym_use_gtest )
ym_use_gtest ()


# ===================================================================
# ヘッダファイルの検査
# ===================================================================
include (CheckIncludeFileCXX)
check_include_file_cxx (stdlib.h    HAVE_STDLIB_H)
check_include_file_cxx (string.h    HAVE_STRING_H)
#check_include_file_cxx (strings.h   HAVE_STRINGS_H)
check_include_file_cxx (unistd.h    HAVE_UNISTD_H)
check_include_file_cxx (limits.h    HAVE_LIMITS_H)
check_include_file_cxx (float.h     HAVE_FLOAT_H)
check_include_file_cxx (math.h      HAVE_MATH_H)
check_include_file_cxx (sys/time.h  HAVE_SYS_TIME_H)
check_include_file_cxx (time.h      HAVE_TIME_H)
check_include_file_cxx (sys/stat.h  HAVE_SYS_STAT_H)
check_include_file_cxx (pwd.h       HAVE_PWD_H)
check_include_file_cxx (sys/param.h HAVE_SYS_PARAM_H)

if ( HAVE_STDLIB_H )
  set ( INCLUDE_STDLIB "#include <stdlib.h>" )
else ()
  set ( INCLUDE_STDLIB )
endif ()

if ( HAVE_STRING_H )
  set ( INCLUDE_STRING "#include <string.h>" )
else ()
  set ( INCLUDE_STRING )
endif ()

if ( HAVE_UNISTD_H )
  set ( INCLUDE_UNISTD "#include <unistd.h>" )
else ()
  set ( INCLUDE_UNISTD )
endif ()

if ( HAVE_LIMITS_H )
  set ( INCLUDE_LIMITS "#include <limits.h>" )
else ()
  set ( INCLUDE_LIMITS )
endif ()

if ( HAVE_FLOAT_H )
  set ( INCLUDE_FLOAT "#include <float.h>" )
else ()
  set ( INCLUDE_FLOAT )
endif ()

if ( HAVE_MATH_H )
  set ( INCLUDE_MATH "#include <math.h>" )
else ()
  set ( INCLUDE_MATH )
endif ()


# ===================================================================
# ライブラリ関数の検査
# ===================================================================
include (CheckFunctionExists)
check_function_exists (getrusage    HAVE_GETRUSAGE)
check_function_exists (times        HAVE_TIMES)


# ===================================================================
# 組み込み型のサイズの検査
# ===================================================================
include (CheckTypeSize)
check_type_size (short       SHORT_SIZE)
check_type_size (int         INT_SIZE)
check_type_size (long        LONG_SIZE)
check_type_size ("long long" LLONG_SIZE)
check_type_size ("void *"    VOIDP_SIZE)

if ( SHORT_SIZE EQUAL 2 )
  set( INT16_TYPE short )
  set( UINT16_TYPE "unsigned short" )
elseif ( INT_SIZE EQUAL 2 )
  set( INT16_TYPE int )
  set( UINT16_TYPE "unsigned int" )
else ()
  # エラー
  message ( FATAL_ERROR
    "No valid type for 16-bit integer" )
endif ()

if ( SHORT_SIZE EQUAL 4 )
  set( INT32_TYPE short )
  set( UINT32_TYPE "unsigned short" )
elseif ( INT_SIZE EQUAL 4 )
  set( INT32_TYPE int )
  set( UINT32_TYPE "unsigned int" )
elseif ( LONG_SIZE EQUAL 4 )
  set( INT32_TYPE long )
  set( UINT32_TYPE "unsigned long" )
else ()
  # エラー
  message ( FATAL_ERROR
    "No valid type for 32-bit integer" )
endif ()

if ( INT_SIZE EQUAL 8 )
  set( INT64_TYPE int )
  set( UINT64_TYPE "unsigned int" )
elseif ( LONG_SIZE EQUAL 8 )
  set( INT64_TYPE long )
  set( UINT64_TYPE "unsigned long" )
elseif ( LLONG_SIZE EQUAL 8 )
  set( INT64_TYPE "lon long" )
  set( UINT64_TYPE "unsigned long long" )
else ()
  # エラー
  message ( FATAL_ERROR
    "No valid type for 64-bit integer" )
endif ()

if ( INT_SIZE EQUAL VOIDP_SIZE )
  set( PTRINT_TYPE "unsigned int" )
elseif ( LONG_SIZE EQUAL VOIDP_SIZE )
  set( PTRINT_TYPE "unsigned long" )
elseif ( LLONG_SIZE EQUAL VOIDP_SIZE )
  set( PTRINT_TYPE "unsigned long long" )
else ()
  # エラー
  message ( FATAL_ERROR
    "No valid type for pointer size integer" )
endif ()


# ===================================================================
# インクルードパスの設定
#
# このディレクトリにはコンパイルするターゲットはないが，
# サブディレクトリに情報を伝えるためにここで設定しておく
# ===================================================================

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================
add_subdirectory (libym_utils)
add_subdirectory (libym_algo)
add_subdirectory (libym_logic)
add_subdirectory (libym_cell)
add_subdirectory (libym_verilog)
add_subdirectory (libym_networks)


# ===================================================================
# YmTools.h の生成
# ===================================================================

configure_file(
  "${PROJECT_SOURCE_DIR}/etc/YmTools.h.in"
  "${PROJECT_BINARY_DIR}/YmTools.h"
  )

configure_file(
  "${PROJECT_SOURCE_DIR}/etc/YmTools_int.h.in"
  "${PROJECT_BINARY_DIR}/YmTools_int.h"
  )


# ===================================================================
# パッケージコンフィグレーションファイルの生成
# ===================================================================

configure_file(
  ${PROJECT_SOURCE_DIR}/etc/YmToolsConfig.cmake.in
  ${PROJECT_BINARY_DIR}/YmToolsConfig.cmake
  @ONLY
  )

configure_file(
  ${PROJECT_SOURCE_DIR}/etc/YmToolsConfig-version.cmake.in
  ${PROJECT_BINARY_DIR}/YmToolsConfig-version.cmake
  @ONLY
  )


# ===================================================================
# インストールターゲット
# ===================================================================

# YmTools.h ヘッダ
install (
  FILES "${PROJECT_BINARY_DIR}/YmTools.h"
  DESTINATION include
  )

# verilog の API 用ヘッダ
foreach ( _header "acc_user.h" "veriuser.h" "vpi_user.h" )
  install (
    FILES "${PROJECT_SOURCE_DIR}/include/${_header}"
    DESTINATION include
    )
endforeach ()

# YmTools のエクスポート用 cmake ファイル
install (
  EXPORT ymtools-targets
  DESTINATION lib/ymtools-${YMTOOLS_VERSION}
  )

# YmTools のインポート用 cmake ファイル
install (FILES
  ${PROJECT_BINARY_DIR}/YmToolsConfig.cmake
  ${PROJECT_BINARY_DIR}/YmToolsConfig-version.cmake
  DESTINATION lib/ymtools-${YMTOOLS_VERSION}
  )


# ===================================================================
# doxygen 起動用のコマンド
# ===================================================================

if (DOXYGEN_FOUND)
  # Doxyfile の生成
  configure_file (
    "${PROJECT_SOURCE_DIR}/etc/Doxyfile.in"
    "${PROJECT_BINARY_DIR}/Doxyfile"
    )

  # doxygen 用ターゲット
  add_custom_target(dox ALL
    DEPENDS "${PROJECT_BINARY_DIR}/docs/html/index.html"
    )

  # doxygen の対象となるヘッダファイル(のパタン)を YM_DOXY_SRCS に入れる．
  set ( YM_DOXY_SRCS
    "${PROJECT_BINARY_DIR}/YmTools.h"
    )
  foreach (dir
      "etc/doxygen-src"
      "include"
      "include/YmAlgo"
      "include/YmCell"
      "include/YmLogic"
      "include/YmNetworks"
      "include/YmUtils"
      "include/YmVerilog"
      )
    list ( APPEND YM_DOXY_SRCS ${dir} )
  endforeach()

  # doxygen 起動ルール
  add_custom_command(
    COMMAND "${DOXYGEN_EXECUTABLE}" ">" "${PROJECT_BINARY_DIR}/doxygen.log"
    DEPENDS "${PROJECT_BINARY_DIR}/Doxyfile" ${YM_DOXY_SRCS}
    OUTPUT "${PROJECT_BINARY_DIR}/docs/html/index.html"
    COMMENT "generating doxygen documents"
    )

  # 生成物のインストールルール
  install (DIRECTORY
    ${PROJECT_BINARY_DIR}/html
    DESTINATION share/doc/ymtools-${YMTOOLS_VERSION}/
    )

endif (DOXYGEN_FOUND)

# ===================================================================
# end-of-CMakeLists.txt
# ===================================================================
