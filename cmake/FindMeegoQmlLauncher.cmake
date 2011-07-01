# - Find MeeGo Qml Launcher
#
# This module defines
#  MEEGO_QMLLAUNCHER_INCLUDE_DIR, where to find png.h, etc.
#  MEEGO_QMLLAUNCHER_LIBRARY, the library to link against to use PNG.
#  MEEGO_QMLLAUNCHER_FOUND, If false, do not try to use PNG.

find_path(MEEGO_QMLLAUNCHER_INCLUDE_DIR 
    meegoqmllauncher.h
    PATH_SUFFIXES meegoqmllauncher
)

find_library(MEEGO_QMLLAUNCHER_LIBRARY NAMES meegoqmllauncher )

find_package_handle_standard_args(MeegoQmlLauncher DEFAULT_MSG MEEGO_QMLLAUNCHER_INCLUDE_DIR MEEGO_QMLLAUNCHER_LIBRARY)

mark_as_advanced(MEEGO_QMLLAUNCHER_INCLUDE_DIR MEEGO_QMLLAUNCHER_LIBRARY )
