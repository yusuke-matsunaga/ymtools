find_package(PkgConfig QUIET)
pkg_search_module(PC_POPT QUIET popt)

# Find the include directories
FIND_PATH(POPT_INCLUDE_DIR
    NAMES popt.h
    HINTS
          ${PC_POPT_INCLUDEDIR}
          ${PC_POPT_INCLUDE_DIRS}
    DOC "Path containing the popt.h include file"
    )

FIND_LIBRARY(POPT_LIBRARY
    NAMES popt
    HINTS
          ${PC_POPT_LIBRARYDIR}
          ${PC_POPT_LIBRARY_DIRS}
    DOC "popt library path"
    )

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(POPT
  REQUIRED_VARS POPT_INCLUDE_DIR POPT_LIBRARY
  VERSION_VAR PC_POPT_VERSION)

MARK_AS_ADVANCED(POPT_INCLUDE_DIR POPT_LIBRARY)
