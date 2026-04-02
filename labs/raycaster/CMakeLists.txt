cmake_minimum_required(VERSION 3.31)
project(raycaster)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Multimedia)
qt_standard_project_setup()

qt_add_executable(raycaster
        application.qrc
        src/main.cpp
        src/raycaster.cpp
        src/ray.cpp
        src/polygon.cpp
        src/camera.cpp
)

qt_add_qml_module(raycaster
        URI path
        VERSION 1.0
        QML_FILES qml/main.qml
)

target_link_libraries(raycaster PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Multimedia
)

target_include_directories(raycaster PRIVATE src)
