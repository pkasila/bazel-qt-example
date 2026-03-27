load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "mainwindow",
    srcs = [
        "mainwindow.cpp",
        "canvas/canvas.cpp",
        "canvas/controller.cpp",
        "geom/polygon.cpp",
        "geom/ray.cpp",
        "geom/misc.cpp",
    ],
    hdrs = [
        "mainwindow.h",
        "canvas/canvas.h",
        "canvas/controller.h",
        "geom/polygon.h",
        "geom/ray.h",
        "geom/misc.h",
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
