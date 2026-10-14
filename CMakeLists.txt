cmake_minimum_required(VERSION 3.16)
project(blackjack VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Dtk6Widget REQUIRED)

add_executable(blackjack
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/BlackjackWidget.cpp
    src/BlackjackWidget.h
    src/Carta.h
    src/Mazo.h
)

target_link_libraries(blackjack PRIVATE
    Qt6::Widgets
    Dtk6::Widget
)

install(TARGETS blackjack DESTINATION bin)
