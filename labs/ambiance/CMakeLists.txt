cmake_minimum_required(VERSION 3.31)
project(ambiance)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Multimedia)
qt_standard_project_setup()

set(TOML11_PRECOMPILE ON)
add_subdirectory(external/toml11 EXCLUDE_FROM_ALL)

qt_add_executable(ambiance
        application.qrc
        src/main.cpp
        src/ambiance.cpp
        src/sound.cpp
        src/player_loader.cpp
)

qt_add_qml_module(ambiance
        URI path
        VERSION 1.0
        QML_FILES qml/main.qml
)

target_link_libraries(ambiance PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Multimedia
        toml11::toml11
)

target_include_directories(ambiance PRIVATE
        src
        external/toml11/include
)
