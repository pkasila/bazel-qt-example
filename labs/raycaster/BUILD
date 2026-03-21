load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "raycasterLib",
    srcs = [
        "canvas.cpp",
        "controller.cpp",
        "mainwindow.cpp",
    ],
    hdrs = [
        "canvas.h",
        "controller.h",
        "mainwindow.h",
        "polygon.h",
        "ray.h",
    ],
    data = [
        "light.jpg",
        "shadow.jpg",
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
    deps = [":raycasterLib"],
)
