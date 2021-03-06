# ===================================================================
# libym_utils のための CMakeLists.txt
# ===================================================================


# ===================================================================
# パッケージの検査
# ===================================================================
include (FindBZip2)
include (FindLibLZMA)
include (FindZLIB)

include (FindPOPT)


# ===================================================================
# インクルードパスの設定
# ===================================================================

include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (ZLIB_FOUND)
  include_directories(${ZLIBINCLUDE_DIRS})
endif (ZLIB_FOUND)

if (BZIP2_FOUND)
  include_directories(${BZIP2_INCLUDE_DIRS})
endif (BZIP2_FOUND)

if (LIBLZMA_FOUND)
  include_directories(${LIBLZMA_INCLUDE_DIRS})
endif (LIBLZMA_FOUND)

if (POPT_FOUND)
  include_directories(${POPT_INCLUDE_DIR})
endif (POPT_FOUND)


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory(gtest)


# ===================================================================
# コンフィグレーション依存のファイルの生成
# ===================================================================
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/etc/CodecTypeConf.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/CodecTypeConf.h"
  )


# ===================================================================
#  ソースファイルの設定
# ===================================================================
set (alloc_SOURCES
  src/alloc/Alloc.cc
  src/alloc/FragAlloc.cc
  src/alloc/SimpleAlloc.cc
  src/alloc/UnitAlloc.cc
  )

set (file_SOURCES
  src/file/File.cc
  src/file/FileInfo.cc
  src/file/FileInfoMgr.cc
  )

set (gen_SOURCES
  src/gen/CombiGen.cc
  src/gen/GenBase.cc
  src/gen/MultiCombiGen.cc
  src/gen/MultiGenBase.cc
  src/gen/MultiPermGen.cc
  src/gen/MultiSetCombiGen.cc
  src/gen/MultiSetGenBase.cc
  src/gen/MultiSetPermGen.cc
  src/gen/PermGen.cc
  )

set (io_SOURCES
  src/io/CodecType.cc
  src/io/FileBuff.cc
  src/io/FileBuff.h
  src/io/FileCoder.cc
  src/io/FileDecoder.cc
  src/io/FileIDO.cc
  src/io/FileODO.cc
  src/io/IDO.cc
  src/io/ODO.cc
  src/io/StreamIDO.cc
  src/io/StringIDO.cc
  src/io/StrListIDO.cc
  src/io/ThruCoder.cc
  src/io/ThruDecoder.cc
  src/io/ZCoder.cc
  src/io/ZDecoder.cc
  )

if (ZLIB_FOUND)
  set (io_SOURCES ${io_SOURCES}
    src/io/GzCoder.cc
    src/io/GzDecoder.cc
    src/io/zstream.cc
    )
endif (ZLIB_FOUND)

if (BZIP2_FOUND)
  set (io_SOURCES ${io_SOURCES}
    src/io/BzCoder.cc
    src/io/BzDecoder.cc
    )
endif (BZIP2_FOUND)

if (LIBLZMA_FOUND)
  set (io_SOURCES ${io_SOURCES}
    src/io/LzmaCoder.cc
    src/io/LzmaDecoder.cc
    )
endif (LIBLZMA_FOUND)

set (itvl_SOURCES
  src/itvl/ItvlMgr.cc
  src/itvl/ItvlMgrImpl.cc
  src/itvl/NameMgr.cc
  )

set (misc_SOURCES
  src/misc/Binder.cc
  src/misc/DlList.cc
  src/misc/MFSet.cc
  src/misc/MpInt.cc
  src/misc/RandGen.cc
  src/misc/RandSampler.cc
  src/misc/RefCount.cc
  src/misc/StopWatch.cc
  )

set (msg_SOURCES
  src/msg/MsgHandler.cc
  src/msg/MsgMgr.cc
  src/msg/MsgMgrImpl.cc
  )

if (POPT_FOUND)
  set (popt_SOURCES
    src/popt/PoptMainApp.cc
    )
else (POPT_FOUND)
  set (popt_SOURCES)
endif (POPT_FOUND)

set (textproc_SOURCES
  src/textproc/Scanner.cc
  src/textproc/StrBuff.cc
  src/textproc/StrPool.cc
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================

ym_add_library (ym_utils
  SHARED
  ${alloc_SOURCES}
  ${file_SOURCES}
  ${gen_SOURCES}
  ${io_SOURCES}
  ${itvl_SOURCES}
  ${misc_SOURCES}
  ${msg_SOURCES}
  ${popt_SOURCES}
  ${textproc_SOURCES}
  )

if (ZLIB_FOUND)
  list (APPEND libs ${ZLIB_LIBRARIES})
endif (ZLIB_FOUND)

if (BZIP2_FOUND)
  list (APPEND libs ${BZIP2_LIBRARIES})
endif (BZIP2_FOUND)

if (LIBLZMA_FOUND)
  list (APPEND libs ${LIBLZMA_LIBRARIES})
endif (LIBLZMA_FOUND)

if (POPT_FOUND)
  list (APPEND libs ${POPT_LIBRARY})
endif (POPT_FOUND)

foreach (_libname ym_utils ym_utils_p ym_utils_d)
  target_link_libraries (${_libname} ${libs})
endforeach ()

if (GPERFTOOLS_FOUND)
  target_link_libraries(ym_utils_p
    ${GPERFTOOLS_LIBRARIES}
    )
endif (GPERFTOOLS_FOUND)


# ===================================================================
#  インストールターゲットの設定
# ===================================================================

install(
  TARGETS ym_utils ym_utils_p ym_utils_d
  DESTINATION lib
  EXPORT ymtools-targets
  )

install(
  DIRECTORY YmUtils
  DESTINATION include
  )
