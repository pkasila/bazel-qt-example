load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library", "qt_resource", "qt_resource_via_qrc", "qt_ui_library")

qt_cc_library(
    name = "mainwindow",
    srcs = [
        "mainwindow.cpp",
        "canvas.cpp",
        "controller.cpp",
        "polygon.cpp",
        "ray.cpp"
        ],
    hdrs = [
        "mainwindow.h",
        "canvas.h",
        "controller.h",
        "polygon.h",
        "ray.h"
    ],
defines = ["QT_NO_KEYWORDS"],

    deps = [
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
    ],
)

qt_cc_binary(
    name = "raycaster",
    srcs = ["main.cpp"],
        defines = ["QT_NO_KEYWORDS"],

    deps = [
        ":mainwindow",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)