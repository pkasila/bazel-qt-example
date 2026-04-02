load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "mainwindow_lib",
    srcs = [
        "mainwindow.cpp",
    ],
    hdrs = [
        "mainwindow.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "lab1_app",
    srcs = [
        "main.cpp",
    ],
    deps = [
        ":mainwindow_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)