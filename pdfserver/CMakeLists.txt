include_directories(${POPPLER_QT4_INCLUDE_DIR})
set(pdfserver_SRCS
    main.cpp
    qtlocalpeer.cpp
    qtlockedfile.cpp
    qtsinglecoreapplication.cpp
    PdfServer.cpp
    PdfServerThread.cpp
    PdfDocument.cpp
    PdfDocumentCache.cpp
)

set(pdfserver_headers
    qtlocalpeer.h
    qtlockedfile.h
    qtsinglecoreapplication.h
    PdfServer.h
    PdfServerThread.h
    PdfDocument.h
    PdfDocumentCache.h
)

kde4_add_executable(pdfserver ${pdfserver_SRCS})
target_link_libraries(pdfserver
    ${QT_QTCORE_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${POPPLER_QT4_LIBRARIES}
)

set_target_properties(pdfserver PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION})

install(TARGETS pdfserver ${INSTALL_TARGETS_DEFAULT_ARGS})
add_subdirectory(tests)
