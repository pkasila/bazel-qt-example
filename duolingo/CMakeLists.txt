cmake_minimum_required(VERSION 3.16)
project(LinguaQuest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Multimedia QUIET)
if (Qt6_FOUND)
    set(QT_LIBS Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Multimedia)
else()
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Multimedia)
    set(QT_LIBS Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Multimedia)
endif()

add_executable(duolingo main.cpp mainwindow.cpp mainwindow.h)
target_link_libraries(duolingo PRIVATE ${QT_LIBS})

add_custom_command(TARGET duolingo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/correct.wav
        ${CMAKE_CURRENT_SOURCE_DIR}/wrong.wav
        $<TARGET_FILE_DIR:duolingo>
)
