load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_qt//:qt.bzl", "qt_cc_library", "qt_ui_library")


qt_ui_library(
    name = "mainwindow_ui",
    ui = "mainwindow.ui",
    deps = [
        "@rules_qt//:qt_widgets",
    ],
)


qt_cc_library(
    name = "task2_lib",
    srcs = [
        "mainwindow.cpp",
        "Results.cpp",
    ],
    hdrs = [
        "mainwindow.h",
        "Results.h",
    ],
    copts = [
        "-Iexternal/rules_qt",
        "-I./labs/basics/task2",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":mainwindow_ui",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

cc_binary(
    name = "basics",
    srcs = ["main.cpp"],
    deps = [
        ":task2_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)
