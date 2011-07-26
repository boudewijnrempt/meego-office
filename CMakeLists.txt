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

option(WITH_MEEGO_QMLLAUNCHER "Support MeeGo QML Launcher for creating the Application and UI." ON)

# Find Calligra
find_package(CalligraLibs REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(QtSparql REQUIRED)

if(WITH_MEEGO_QMLLAUNCHER)
    find_package(QtMobility COMPONENTS Sensors REQUIRED)
    find_package(MeegoQmlLauncher REQUIRED)
    add_definitions(-DUSE_MEEGO_QMLLAUNCHER)
endif()

include_directories(${QT_INCLUDES} ${KDE4_INCLUDES} ${QSPARQL_INCLUDE_DIRS}
                    ${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES} ${WORDS_INCLUDES}
                    ${TABLES_INCLUDES} ${STAGE_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}
                    ${QTMOBILITY_INCLUDE_DIRS} ${MEEGO_QML_LAUNCHER_INCLUDE_DIR}
                   )

set(PDF_SERVER_BINARY /usr/bin/pdfserver CACHE PATH "Path to PDF Server binary")
configure_file(pdf/PDFServerConfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/PDFServerConfig.h)

set(meego-office-suite_SRCS
    shared/CanvasInputProxy.cpp
    shared/CanvasControllerDeclarative.cpp
    shared/IconImageProvider.cpp
    shared/MainWindow.cpp
    shared/DocumentListModel.cpp
    shared/DocumentThumbnailListModel.cpp
    shared/PageThumbnailProvider.cpp
    shared/ProgressProxy.cpp
    shared/TextSelection.cpp

    words/WordsCanvas.cpp

    tables/TablesCanvas.cpp
    tables/TablesHeader.cpp

    stage/StageCanvas.cpp
    stage/StageDeclarativeView.cpp

    pdf/PDFCanvas.cpp
    pdf/PDFDocument.cpp
    pdf/PDFCanvasController.cpp
    pdf/PDFProcessManager.cpp
    pdf/PDFPage.cpp
    pdf/PDFSelection.cpp
    pdf/PDFSearch.cpp
    
    main.cpp
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
    ui/DocumentThumbnailList.qml
    ui/ZoomControl.qml
    ui/AboutDialog.qml
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
install(PROGRAMS meego-office-suite.desktop DESTINATION
${CMAKE_INSTALL_PREFIX}/share/applications)
