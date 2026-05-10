load("@rules_pkg//pkg:tar.bzl", "pkg_tar")
load("@rules_qt//:qt.bzl", "qt_cc_binary")

qt_cc_binary(
    name = "app",
    srcs = [
        "controller.cpp",
        "controller.h",
        "drawing_widget.cpp",
        "drawing_widget.h",
        "main.cpp",
        "mainwindow.cpp",
        "mainwindow.h",
        "polygon.cpp",
        "polygon.h",
        "ray.cpp",
        "ray.h",
        "validator.h"
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
    linkopts = [
        "-lgmp",
        "-lmpfr",
    ],
    copts = [
        "-frounding-math",
    ],
)

pkg_tar(
    name = "release",
    srcs = [
        ":app",
    ],
    include_runfiles = True,
    package_dir = "app",
)
