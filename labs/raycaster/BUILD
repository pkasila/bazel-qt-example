load("@rules_qt//:qt.bzl", "qt_cc_binary")

qt_cc_binary(
    name = "raycaster",
    srcs = [
        "main.cpp",
        "mainwindow.cpp",
        "renderwidget.cpp",
        "controller.cpp",
        "polygon.cpp",
        "ray.cpp",
        "mainwindow.h",
        "renderwidget.h",
        "controller.h",
        "polygon.h",
        "ray.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_gui",
    ],
)
