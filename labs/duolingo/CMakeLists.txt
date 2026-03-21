cmake_minimum_required(VERSION 3.29)
project(untitled)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "F:/QtInstall/6.9.0/mingw_64/")

find_package(Qt6 COMPONENTS
  Core
  Gui
  Widgets
  Sql
  Multimedia
  REQUIRED)

add_executable(untitled main.cpp
        MainWindow.cpp
        MainWindow.h
        ExerciseView.cpp
        ExerciseView.h
        Exercise.cpp
        Exercise.h
        TranslationExercise.cpp
        TranslationExercise.h
        GrammarExercise.cpp
        GrammarExercise.h
        MainMenu.cpp
        MainMenu.h
        resources.qrc
)
target_link_libraries(untitled
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::Sql
  Qt6::Multimedia
)

add_custom_command(
        TARGET untitled POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/mydata.db"
        "$<TARGET_FILE_DIR:untitled>/mydata.db"
)
