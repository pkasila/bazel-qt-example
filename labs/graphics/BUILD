load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "main_window",
    srcs = ["mainwindow.cpp", "ray.cpp", "polygon.cpp", "controller.cpp"],
    hdrs = ["mainwindow.h", "ray.h", "polygon.h", "controller.h"],
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
        ":main_window",
        "@rules_qt//:qt_widgets",
    ],
)