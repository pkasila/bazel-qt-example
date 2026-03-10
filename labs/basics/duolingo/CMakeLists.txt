cmake_minimum_required(VERSION 3.16)
project(language_learning)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS 
    Core
    Gui
    Widgets
    Multimedia
)

set(PROJECT_SOURCES
    main.cpp
    main_window.cpp
    main_window.h
    exercise.h
    resources.qrc
)

add_executable(language_learning ${PROJECT_SOURCES})

target_link_libraries(language_learning PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
)

# Copy sound files to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sounds DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Set include directories
target_include_directories(language_learning PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set compiler flags
if(MSVC)
    target_compile_options(language_learning PRIVATE /W4)
else()
    target_compile_options(language_learning PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install target
install(TARGETS language_learning
    RUNTIME DESTINATION bin
) 