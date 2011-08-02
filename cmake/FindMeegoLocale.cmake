# - Find MeeGo Locale Library
#
# This module defines
#  MEEGO_LOCALE_INCLUDE_DIR - Include directory for MeeGo Locale.
#  MEEGO_LOCALE_INCLUDES - All includes needed to use MeeGo Locale.
#  MEEGO_LOCALE_LIBRARY - MeeGo Locale library to link against.
#  MEEGO_LOCALE_FOUND - Whether MeeGo Locale was found or not.

find_path(MLITE_INCLUDE_DIR
    MGConfItem
    PATH_SUFFIXES mlite
)

find_path(MEEGO_LOCALE_INCLUDE_DIR
    meegolocale.h
)

set(MEEGO_LOCALE_INCLUDES
    ${MEEGO_LOCALE_INCLUDE_DIR}
    ${MLITE_INCLUDE_DIR}
)

find_library(MEEGO_LOCALE_LIBRARY NAMES meegolocale )

find_package_handle_standard_args(MeegoLocale DEFAULT_MSG MEEGO_LOCALE_INCLUDE_DIR MEEGO_LOCALE_LIBRARY)

mark_as_advanced(MEEGO_LOCALE_INCLUDE_DIR MEEGO_LOCALE_LIBRARY )
