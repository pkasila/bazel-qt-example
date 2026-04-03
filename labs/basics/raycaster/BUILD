load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "graphics_lib",
    srcs = [
        "mainwindow.cpp",
        "polygon.cpp",
        "ray.cpp",
        "canvaswidget.cpp",
        "controller.cpp",
    ],
    hdrs = [
        "mainwindow.h",
        "polygon.h",
        "ray.h",
        "canvaswidget.h",
        "controller.h",
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
        ":graphics_lib",
        "@rules_qt//:qt_widgets",
    ],
)