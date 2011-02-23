include_directories(${CMAKE_SOURCE_DIR}/tables)

set(tables_mobile_SRCS
    CMTablesCanvas.cpp
    main.cpp
)

set(tables_mobile_ui_SRCS
    ui/Tables.qml
    ui/TableView.qml
)

kde4_add_executable(calligratables_mobile ${tables_mobile_SRCS})
target_link_libraries(calligratables_mobile ${CALLIGRA_MOBILE_LIBRARIES} calligratablescommon)

install(TARGETS calligratables_mobile ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${tables_mobile_ui_SRCS} DESTINATION ${DATA_INSTALL_DIR}/calligratables_mobile)
install(PROGRAMS calligratables_mobile.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})