load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "mainwindow",
    srcs = [
        "mainwindow.cpp",
        "countwidget.cpp",
        "progresswidget.cpp",
        "questionwidget.cpp",
        "viewwidget.cpp",
    ],
    hdrs = [
        "mainwindow.h",
        "countwidget.h",
        "viewwidget.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        ":mainwindow",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)