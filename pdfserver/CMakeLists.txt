project(PdfServer)
cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(PopplerQt4 REQUIRED)

include_directories(${QT_INCLUDES} ${POPPLER_QT4_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

option(BUILD_TESTS "Build unit tests" ON)

set(pdfserver_SRCS
    main.cpp
    qtlocalpeer.cpp
    qtlockedfile.cpp
    qtsinglecoreapplication.cpp
    PdfServer.cpp
    PdfServerThread.cpp
    PdfDocument.cpp
    PdfDocumentCache.cpp
    PdfReply.cpp
)

set(pdfserver_headers
    qtlocalpeer.h
    qtlockedfile.h
    qtsinglecoreapplication.h
    PdfServer.h
    PdfServerThread.h
    PdfDocument.h
    PdfDocumentCache.h
    PdfReply.h
)

qt4_automoc(${pdfserver_SRCS})
add_executable(pdfserver ${pdfserver_SRCS})
target_link_libraries(pdfserver
    ${QT_QTCORE_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${POPPLER_QT4_LIBRARIES}
)

install(TARGETS pdfserver RUNTIME DESTINATION bin)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
