include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/.. } ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DPDF_SERVER_EXECUTABLE="\\"${CMAKE_CURRENT_BINARY_DIR}/../pdfserver\\"")
add_definitions(-DPDF_TEST_FILE="\\"${CMAKE_CURRENT_SOURCE_DIR}/lorem.pdf\\"")
add_definitions(-DPDF_TEST_FILE_LINK="\\"${CMAKE_CURRENT_SOURCE_DIR}/onelink.pdf\\"")

set(pdfservertest_SRCS PdfServerTest.cpp)
qt4_automoc(${pdfservertest_SRCS})
add_executable(PdfServerTest ${pdfservertest_SRCS})
target_link_libraries(PdfServerTest  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTNETWORK_LIBRARY})

