
# Use FIND_PACKAGE( QtMobility COMPONENTS ... ) to enable modules
IF( QtMobility_FIND_COMPONENTS )
  FOREACH( component ${QtMobility_FIND_COMPONENTS} )
    STRING( TOUPPER ${component} _COMPONENT )
    SET( QT_MOBILITY_USE_${_COMPONENT} 1 )
  ENDFOREACH( component )
ENDIF( QtMobility_FIND_COMPONENTS )

INCLUDE(CheckSymbolExists)
INCLUDE(MacroAddFileDependencies)

SET(QT_MOBILITY_MODULES Bearer Contacts Location Messaging Media
                        PublishSubscribe ServiceFramework Sensors
                        SystemInfo Versit)

find_path(QT_MOBILITY_GLOBAL_INCLUDE_DIR qmobilityglobal.h PATH_SUFFIXES QtMobility)

set(QT_MOBILITY_COMPONENTS "QT_MOBILITY_GLOBAL_INCLUDE_DIR")

foreach(QT_MOBILITY_MODULE ${QT_MOBILITY_MODULES})
    string(TOUPPER ${QT_MOBILITY_MODULE} _qt_mobility_module)
    if(QT_MOBILITY_USE_${_qt_mobility_module})
        message(STATUS "Looking for QtMobility Component ${QT_MOBILITY_MODULE}")
        find_path(QT_MOBILITY_${_qt_mobility_module}_INCLUDE_DIR Q${QT_MOBILITY_MODULE} PATH_SUFFIXES Qt${QT_MOBILITY_MODULE})
        find_library(QT_MOBILITY_${_qt_mobility_module}_LIBRARY NAMES Qt${QT_MOBILITY_MODULE})
        list(APPEND QT_MOBILITY_COMPONENTS "QT_MOBILITY_${_qt_mobility_module}_INCLUDE_DIR" "QT_MOBILITY_${_qt_mobility_module}_LIBRARY")
    endif()
endforeach(QT_MOBILITY_MODULE)

include( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( QtMobility DEFAULT_MSG ${QT_MOBILITY_COMPONENTS} )
