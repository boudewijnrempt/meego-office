include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/.. })
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions(-DPDF_SERVER_EXECUTABLE="\\"${CMAKE_CURRENT_BINARY_DIR}/../pdfserver\\"")
add_definitions(-DPDF_TEST_FILE="\\"${CMAKE_CURRENT_SOURCE_DIR}/lorem.pdf\\"")

set(pdfservertest_SRCS PdfServerTest.cpp)
kde4_add_unit_test(PdfServerTest TESTNAME intel-mobile-pdfserver-PdfServerTest ${pdfservertest_SRCS})
target_link_libraries(PdfServerTest  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTNETWORK_LIBRARY})

