include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/.. })
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions(-DPDF_SERVER_EXECUTABLE="\\"${CMAKE_CURRENT_BINARY_DIR}/../pdfserver\\"")

set(pdfservertest_SRCS PdfServerTest.cpp)
kde4_add_unit_test(PdfServerTest TESTNAME intel-mobile-pdfserver-PdfServerText ${pdfservertest_SRCS})
target_link_libraries(PdfServerTest  ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

