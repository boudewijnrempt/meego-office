include_directories(${CMAKE_SOURCE_DIR}/kpresenter ${CMAKE_SOURCE_DIR}/kpresenter/part ${KOPAGEAPP_INCLUDES})

set(stage_mobile_SRCS
    CMStageCanvas.cpp
    CMStageDeclarativeView.cpp
    main.cpp
)

set(stage_mobile_ui_SRCS
    ui/Stage.qml
    ui/StageView.qml
)

kde4_add_executable(calligrastage_mobile ${stage_mobile_SRCS})
target_link_libraries(calligrastage_mobile ${CALLIGRA_MOBILE_LIBRARIES} kopageapp calligrastageprivate)

install(TARGETS calligrastage_mobile ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${stage_mobile_ui_SRCS} DESTINATION ${DATA_INSTALL_DIR}/calligrastage_mobile)
install(PROGRAMS calligrastage_mobile.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
