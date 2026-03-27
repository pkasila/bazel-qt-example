
cmake_minimum_required(VERSION 3.29)
project(task_1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt modules
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(task_1

        main.cpp
        main_window.cpp
        main_window.h
        Ticket.h)
target_link_libraries(task_1
        Qt::Core
        Qt::Gui
        Qt::Widgets
)