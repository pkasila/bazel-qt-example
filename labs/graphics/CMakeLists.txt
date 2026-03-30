cmake_minimum_required(VERSION 3.29)
project(graphics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(graphics
        main.cpp
        Ray.cpp
        Ray.h
        Polygon.h
        Controller.h
        Controller.cpp
        PaintWidget.cpp
        PaintWidget.h
        Polygon.cpp)
target_link_libraries(graphics
        Qt::Core
        Qt::Gui
        Qt::Widgets
)