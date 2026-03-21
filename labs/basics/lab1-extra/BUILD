load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "mainwindow_lib",
    srcs = ["mainwindow.cpp"],
    hdrs = ["mainwindow.h"],
    deps = [
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
    ],
)

qt_cc_binary(
    name = "lab1-extra",
    srcs = ["main.cpp"],
    deps = [
        ":mainwindow_lib",
        "@rules_qt//:qt_widgets",
    ],
)