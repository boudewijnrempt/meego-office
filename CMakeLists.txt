project(meego-office-suite)

########################
#########################
## Look for KDE and Qt ##
#########################
########################
set(KDE_MIN_VERSION "4.3.0")
find_package(KDE4 4.3.0 REQUIRED)

find_package(Qt4 4.7.1 REQUIRED)

include(KDE4Defaults)

# Find Calligra
find_package(CalligraLibs REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(QtMobility COMPONENTS Sensors REQUIRED)
find_package(MeegoQmlLauncher REQUIRED)
find_package(QtSparql REQUIRED)

include_directories(${QT_INCLUDES} ${KDE4_INCLUDES} ${QSPARQL_INCLUDE_DIRS}
                    ${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES} ${WORDS_INCLUDES}
                    ${TABLES_INCLUDES} ${STAGE_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}
                    ${QTMOBILITY_INCLUDE_DIRS} ${MEEGO_QML_LAUNCHER_INCLUDE_DIR}
                   )

set(PDF_SERVER_BINARY ${CMAKE_CURRENT_BINARY_DIR}/pdfserver/pdfserver CACHE PATH "Path to PDF Server binary")
configure_file(pdf/PDFServerConfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/PDFServerConfig.h)

set(meego-office-suite_SRCS
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
    tables/CMTablesHeader.cpp

    stage/CMStageCanvas.cpp
    stage/CMStageDeclarativeView.cpp

    pdf/PDFCanvas.cpp
    pdf/PDFDocument.cpp
    pdf/PDFCanvasController.cpp
    pdf/PDFProcessManager.cpp
    pdf/PDFPage.cpp
    pdf/PDFSelection.cpp
    
    main.cpp
)

set(meego-office-suite_ui_components_SRCS
    components/qmldir
    components/DocumentThumbnailList.qml
    components/ZoomControl.qml
)

set(meego-office-suite_ui_SRCS
    ui/main.qml
    ui/DocumentPicker.qml
    ui/Marker.qml
    ui/TextCopiedToClipboardMessage.qml
    ui/ViewLoader.qml
    ui/DocumentsPage.qml
    ui/ViewDocumentPage.qml
    ui/Settings.qml
    ui/ViewToolBar.qml
    ui/SearchToolBar.qml
)

kde4_add_executable(meego-office-suite ${meego-office-suite_SRCS})
target_link_libraries(meego-office-suite
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KFILE_LIBS}
    ${QTSPARQL_LIBRARY}
    ${FLAKE_LIBRARY}
    ${KOWIDGETS_LIBRARY}
    ${KOPAGEAPP_LIBRARY}
    wordsprivate
    calligratablescommon
    calligrastageprivate
    ${QTMOBILITY_LIBRARIES}
    ${MEEGO_QMLLAUNCHER_LIBRARY}
)

add_subdirectory(icons)

install(TARGETS meego-office-suite ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${meego-office-suite_ui_SRCS} DESTINATION ${DATA_INSTALL_DIR}/meego-office-suite)
install(FILES ${meego-office-suite_ui_components_SRCS} DESTINATION ${DATA_INSTALL_DIR}/meego-office-suite/org/calligra/mobile)
install(PROGRAMS meego-office-suite.desktop DESTINATION
${CMAKE_INSTALL_PREFIX}/share/applications)
