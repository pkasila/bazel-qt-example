load("@rules_qt//qt:qt.bzl", "qt_cc_binary")

qt_cc_binary(
    name = "raycaster",
    srcs = [
        "main.cpp",
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
        "@qt//:QtWidgets",
    ],
)