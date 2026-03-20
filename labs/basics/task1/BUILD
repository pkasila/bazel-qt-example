load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_qt//:qt.bzl", "qt_cc_library", "qt_ui_library")

# Rule to process the .ui file
qt_ui_library(
    name = "mainwindow_ui",
    ui = "mainwindow.ui",
    deps = [
        "@rules_qt//:qt_widgets",
    ],
)

# Library for your code
qt_cc_library(
    name = "task1_lib",
    srcs = [
        "mainwindow.cpp",  # Exclude main.cpp from here, as it will be in cc_binary
    ],
    hdrs = [
        "mainwindow.h",
    ],
    copts = [
        "-Iexternal/rules_qt",  # Path to rules_qt dependencies
        "-I./labs/basics/task1",  # Path to the source folder
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":mainwindow_ui",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

# Executable binary
cc_binary(
    name = "task1",  # Make sure this name matches what you want to run
    srcs = ["main.cpp"],  # main.cpp is included in the binary
    deps = [
        ":task1_lib",  # Include the library that contains the Qt code
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)