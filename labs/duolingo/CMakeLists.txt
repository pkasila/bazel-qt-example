cmake_minimum_required(VERSION 3.16)

project(duolingo_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

set(APP_SOURCES
    main.cpp
    mainwindow.cpp
    difficultydialog.cpp
    questionrepository.cpp
    answerchecker.cpp
    audiohelper.cpp
    appstyle.cpp
)

set(APP_HEADERS
    mainwindow.h
    difficultydialog.h
    question.h
    questionrepository.h
    answerchecker.h
    audiohelper.h
    appstyle.h
)

find_package(Qt6 COMPONENTS Widgets Multimedia QUIET)

if(Qt6_FOUND)
    qt_add_executable(duolingo ${APP_SOURCES} ${APP_HEADERS})
    target_link_libraries(duolingo PRIVATE Qt6::Widgets Qt6::Multimedia)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets Multimedia)
    add_executable(duolingo ${APP_SOURCES} ${APP_HEADERS})
    target_link_libraries(duolingo PRIVATE Qt5::Widgets Qt5::Multimedia)
endif()

foreach(sound_file correct.wav wrong.wav)
    add_custom_command(TARGET duolingo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${sound_file}
            $<TARGET_FILE_DIR:duolingo>/${sound_file}
    )
endforeach()
