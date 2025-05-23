load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "mainwindow",
    srcs = [
        "mainwindow.cpp",
        "controller.cpp",
        "polygon.cpp",
        "ray.cpp",
    ],
    hdrs = [
        "mainwindow.h",
        "controller.h",
        "polygon.h",
        "ray.h",

    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "raycaster",
    srcs = ["main.cpp"],
    deps = [
        ":mainwindow",
        "@rules_qt//:qt_widgets",
    ],
)