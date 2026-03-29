cmake_minimum_required(VERSION 3.29)
project(MyProj)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_compile_definitions(USE_OPENGL)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        OpenGLWidgets
        REQUIRED)

add_executable(Raytracer
        main.cpp
        MainWindow.cpp
        MainWindow.h
        RenderArea.cpp
        RenderArea.h
        Ray.cpp
        Ray.h
        Polygon.cpp
        Polygon.h
        Controller.cpp
        Controller.h
        LightSource.cpp
        LightSource.h
        .qrc
        Settings.cpp
        Settings.h
)
target_link_libraries(Raytracer
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::OpenGLWidgets
)

