# - Find QtSparql
#
# This module defines
#  QTSPARQL_INCLUDE_DIR, where to find png.h, etc.
#  QTSPARQL_LIBRARY, the library to link against to use PNG.
#  QTSPARQL_FOUND, If false, do not try to use PNG.

find_path(QTSPARQL_INCLUDE_DIR 
    QtSparql
    PATH_SUFFIXES QtSparql
)

find_library(QTSPARQL_LIBRARY NAMES QtSparql )

find_package_handle_standard_args(QtSparql DEFAULT_MSG QTSPARQL_INCLUDE_DIR QTSPARQL_LIBRARY)

mark_as_advanced(QTSPARQL_INCLUDE_DIR QTSPARQL_LIBRARY )
