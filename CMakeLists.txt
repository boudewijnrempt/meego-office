project(intel-office-suite)

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
include(MacroAdditionalCleanFiles)
include(MacroAddFileDependencies)
include(FindPkgConfig)

# Find Calligra
find_package(CalligraLibs REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(QtMobility COMPONENTS SystemInfo)

pkg_check_modules(QSPARQL REQUIRED QtSparql)

include_directories(${QT_INCLUDES} ${KDE4_INCLUDES} ${QSPARQL_INCLUDE_DIRS}
                    ${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES} ${WORDS_INCLUDES}
                    ${TABLES_INCLUDES} ${STAGE_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

if(QTMOBILITY_FOUND)
    include_directories(${QT_MOBILITY_GLOBAL_INCLUDE_DIR} ${QT_MOBILITY_SYSTEMINFO_INCLUDE_DIR})
    add_definitions(-DWITH_QT_SYSTEMINFO)
endif(QTMOBILITY_FOUND)

set(PDF_SERVER_BINARY ${CMAKE_CURRENT_BINARY_DIR}/pdfserver/pdfserver CACHE PATH "Path to PDF Server binary")
configure_file(pdf/PDFServerConfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/PDFServerConfig.h)

set(intel-office-suite_SRCS
    CMCanvasInputProxy.cpp
    CMCanvasControllerDeclarative.cpp
    CMIconImageProvider.cpp
    CMMainWindow.cpp
    CMDocumentListModel.cpp
    CMDocumentThumbnailListModel.cpp
    CMPageThumbnailProvider.cpp
    CMProgressProxy.cpp
    CMTextSelection.cpp

    words/CMWordsCanvas.cpp

    tables/CMTablesCanvas.cpp

    stage/CMStageCanvas.cpp
    stage/CMStageDeclarativeView.cpp

    pdf/PDFCanvas.cpp
    pdf/PDFDocument.cpp
    pdf/PDFCanvasController.cpp
    pdf/PDFProcessManager.cpp
    pdf/PDFPage.cpp
    
    main.cpp
)

set(intel-office-suite_ui_components_SRCS
    components/qmldir
    components/DocumentThumbnailList.qml
    components/ZoomControl.qml
)

set(intel-office-suite_ui_SRCS
    ui/main.qml
    ui/DocumentPicker.qml
    ui/Marker.qml
    ui/TextCopiedToClipboardMessage.qml
    ui/ViewLoader.qml
    ui/DocumentsPage.qml
    ui/ViewDocumentPage.qml
    ui/Settings.qml
)

set(intel-office-suite_headers
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

kde4_add_executable(intel-office-suite ${intel-office-suite_SRCS})
target_link_libraries(intel-office-suite
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KFILE_LIBS}
    ${QSPARQL_LIBRARIES}
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
    intel-office-suite
)

if(QTMOBILITY_FOUND)
    target_link_libraries(intel-office-suite ${QT_MOBILITY_SYSTEMINFO_LIBRARY})
    set(CALLIGRA_MOBILE_LIBRARIES ${CALLIGRA_MOBILE_LIBRARIES} ${QT_MOBILITY_SYSTEMINFO_LIBRARY})
endif()

set_target_properties(intel-office-suite PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION})

install(TARGETS intel-office-suite ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${intel-office-suite_ui_SRCS} DESTINATION ${DATA_INSTALL_DIR}/intel-office-suite)
install(FILES ${intel-office-suite_ui_components_SRCS} DESTINATION ${DATA_INSTALL_DIR}/intel-office-suite/org/calligra/mobile)
install(FILES ${intel-office-suite_headers} DESTINATION ${INCLUDE_INSTALL_DIR})
install(PROGRAMS intel-office-suite.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
