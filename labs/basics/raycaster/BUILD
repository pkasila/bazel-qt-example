load("@rules_qt//:qt.bzl", "qt_cc_binary")

qt_cc_binary(
    name = "raycaster",
    srcs = [
        "main.cpp",
        "mainwindow.cpp",
        "mainwindow.h",
        "ray.h",
        "polygon.h",
        "polygon.cpp",
        "controller.h",
        "controller.cpp",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)