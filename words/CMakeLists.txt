set(words_mobile_SRCS
    CMWordsCanvas.cpp
    main.cpp
)

set(words_mobile_ui_SRCS
    ui/Words.qml
    ui/WordsView.qml
)

kde4_add_executable(calligrawords_mobile ${words_mobile_SRCS})
target_link_libraries(calligrawords_mobile ${CALLIGRA_MOBILE_LIBRARIES} wordsprivate)

install(TARGETS calligrawords_mobile ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${words_mobile_ui_SRCS} DESTINATION ${DATA_INSTALL_DIR}/calligrawords_mobile)
install(PROGRAMS calligrawords_mobile.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
