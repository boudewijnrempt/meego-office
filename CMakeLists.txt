project(calligra_mobile)

########################
#########################
## Look for KDE and Qt ##
#########################
########################
set(KDE_MIN_VERSION "4.3.0")
find_package(KDE4 4.3.0 REQUIRED)

find_package(Qt4 4.7.1 REQUIRED)

include(KDE4Defaults)
include(MacroLibrary)
include (MacroAdditionalCleanFiles)
include (MacroAddFileDependencies)

# Find Calligra
find_package(CalligraLibs REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(QtMobility COMPONENTS SystemInfo)

include_directories(${QT_INCLUDES} ${KDE4_INCLUDES} ${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES} ${WORDS_INCLUDES}
                    ${TABLES_INCLUDES} ${STAGE_INCLUDES})

if(QTMOBILITY_FOUND)
    include_directories(${QT_MOBILITY_GLOBAL_INCLUDE_DIR} ${QT_MOBILITY_SYSTEMINFO_INCLUDE_DIR})
    add_definitions(-DWITH_QT_SYSTEMINFO)
endif(QTMOBILITY_FOUND)

set(calligramobile_SRCS
    CMCanvasInputProxy.cpp
    CMCanvasControllerDeclarative.cpp
    CMIconImageProvider.cpp
    CMMainWindow.cpp
    CMDocumentListModel.cpp
    CMDocumentThumbnailListModel.cpp
    CMPageThumbnailProvider.cpp
    CMProgressProxy.cpp

    words/CMWordsCanvas.cpp
    
    tables/CMTablesCanvas.cpp

    stage/CMStageCanvas.cpp
    stage/CMStageDeclarativeView.cpp

    main.cpp
)

set(calligramobile_ui_components_SRCS
    components/qmldir
    components/Action.qml
    components/ActionBar.qml
    components/DocumentThumbnailList.qml
    components/Button.qml
    components/Menu.qml
    components/ScreenOrientation.qml
    components/TitleBar.qml
    components/ToolButton.qml
    components/ProgressBar.qml
)

set(calligramobile_ui_components_RES
    components/resources/arrowhead.png
   )

set(calligramobile_ui_SRCS
    ui/main.qml
    ui/AboutDialog.qml
    ui/DocumentPicker.qml
    ui/Marker.qml
    ui/SearchBar.qml
    ui/TextCopiedToClipboardMessage.qml
    ui/ViewLoader.qml
)

set(calligramobile_ui_RES
    ui/resources/marker.png
)

set(calligramobile_headers
    CMCanvasControllerDeclarative.h
    CMIconImageProvider.h
    CMMainWindow.h
    calligramobile_export.h
    CMDocumentListModel.h

    words/CMWordsCanvas.h

    tables/CMTablesCanvas.h
    
    stage/CMStageCanvas.h
    stage/CMStageDeclarativeView.h
)

kde4_add_executable(calligramobile ${calligramobile_SRCS})
target_link_libraries(calligramobile
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KFILE_LIBS}
    ${FLAKE_LIBRARY}
    ${KOWIDGETS_LIBRARY}
    ${KOPAGEAPP_LIBRARY}

    wordsprivate
    calligratablescommon
    calligrastageprivate
)

set(CALLIGRA_MOBILE_LIBRARIES
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${FLAKE_LIBRARY}
    ${KOWIDGETS_LIBRARY}
    ${KOPAGEAPP_LIBRARY}
    calligramobile
)

if(QTMOBILITY_FOUND)
    target_link_libraries(calligramobile ${QT_MOBILITY_SYSTEMINFO_LIBRARY})
    set(CALLIGRA_MOBILE_LIBRARIES ${CALLIGRA_MOBILE_LIBRARIES} ${QT_MOBILITY_SYSTEMINFO_LIBRARY})
endif()

set_target_properties(calligramobile PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION})

install(TARGETS calligramobile ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${calligramobile_ui_SRCS} DESTINATION ${DATA_INSTALL_DIR}/calligramobile)
install(FILES ${calligramobile_ui_RES} DESTINATION ${DATA_INSTALL_DIR}/calligramobile/resources)
install(FILES ${calligramobile_ui_components_SRCS} DESTINATION ${DATA_INSTALL_DIR}/calligramobile/org/calligra/mobile)
install(FILES ${calligramobile_ui_components_RES} DESTINATION ${DATA_INSTALL_DIR}/calligramobile/org/calligra/mobile/resources)
install(FILES ${calligramobile_headers} DESTINATION ${INCLUDE_INSTALL_DIR})
install(PROGRAMS intel-documents.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

