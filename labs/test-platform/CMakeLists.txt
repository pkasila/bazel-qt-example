cmake_minimum_required(VERSION 3.31)
project(test-platform)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Multimedia)
qt_standard_project_setup()

set(TOML11_PRECOMPILE ON)
add_subdirectory(external/toml11 EXCLUDE_FROM_ALL)

set(WAMR_BUILD_PLATFORM "linux")
set(WAMR_BUILD_TARGET "X86_64")
set(WAMR_BUILD_INTERP 1)
set(WAMR_BUILD_FAST_INTERP 1)
set(WAMR_BUILD_AOT 1)
set(WAMR_BUILD_LIBC_BUILTIN 1)
set(WAMR_BUILD_LIBC_WASI 1)
set(WAMR_BUILD_REF_TYPES 1)
set(WAMR_ROOT_DIR external/wasm-micro-runtime)

include(${WAMR_ROOT_DIR}/build-scripts/runtime_lib.cmake)
include(${SHARED_DIR}/utils/uncommon/shared_uncommon.cmake)
add_library(vmlib ${WAMR_RUNTIME_LIB_SOURCE})

qt_add_executable(test-platform
        application.qrc
        src/main.cpp
        src/test_platform.cpp
        src/test.cpp
        src/question.cpp
        src/resource_manager.cpp
        src/error.cpp
        src/variant.hpp
        src/checkbox.hpp
        ${UNCOMMON_SHARED_SOURCE}
)

qt_add_qml_module(test-platform
        URI path
        VERSION 1.0
        QML_FILES qml/main.qml qml/ErrorDialog.qml
)

target_link_libraries(test-platform PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Multimedia
        toml11::toml11
        vmlib
)

target_include_directories(test-platform PRIVATE
        src
        external/toml11/include
)
