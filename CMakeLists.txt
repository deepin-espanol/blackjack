cmake_minimum_required(VERSION 3.16)
project(blackjack VERSION 0.2.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Dtk6Widget REQUIRED)
find_program(LRELEASE NAMES lrelease lrelease-qt6 HINTS /usr/lib/qt6/bin REQUIRED)

# Traducciones: cada .ts se compila a .qm y se embebe en :/translations
file(GLOB TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/translations/*.ts)
set(QM_FILES "")
set(QRC_ENTRIES "")
foreach(TS_FILE ${TS_FILES})
    get_filename_component(TS_NAME ${TS_FILE} NAME_WE)
    set(QM_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TS_NAME}.qm")
    add_custom_command(
        OUTPUT ${QM_FILE}
        COMMAND ${LRELEASE} -silent ${TS_FILE} -qm ${QM_FILE}
        DEPENDS ${TS_FILE}
        COMMENT "Compiling ${TS_NAME}.ts"
    )
    list(APPEND QM_FILES ${QM_FILE})
    string(APPEND QRC_ENTRIES "        <file>${TS_NAME}.qm</file>\n")
endforeach()
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/translations.qrc"
    "<RCC>\n    <qresource prefix=\"/translations\">\n${QRC_ENTRIES}    </qresource>\n</RCC>\n")

add_executable(blackjack
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/BlackjackWidget.cpp
    src/BlackjackWidget.h
    src/Carta.h
    src/CartaPainter.h
    src/Mazo.h
    src/resources.qrc
    "${CMAKE_CURRENT_BINARY_DIR}/translations.qrc"
    ${QM_FILES}
)

target_link_libraries(blackjack PRIVATE
    Qt6::Widgets
    Dtk6::Widget
)

install(TARGETS blackjack RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES src/org.deepinenespanol.blackjack.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES src/icons/blackjack.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
