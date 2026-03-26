cmake_minimum_required(VERSION 3.31)
project(reviser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
qt_standard_project_setup()

qt_add_executable(reviser
        application.qrc
        src/main.cpp
        src/reviser.cpp
        src/ticket.cpp
)

qt_add_qml_module(reviser
    URI path
    VERSION 1.0
    QML_FILES qml/main.qml
)

target_link_libraries(reviser PRIVATE Qt6::Quick)
target_include_directories(reviser PRIVATE src)
