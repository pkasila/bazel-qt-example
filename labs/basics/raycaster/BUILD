load("@rules_qt//:qt.bzl", "qt_cc_library", "qt_cc_binary")
load("@rules_pkg//pkg:tar.bzl", "pkg_tar")

qt_cc_library(
    name = "raycaster_lib",
    srcs = [
        "controller.cpp",
        "customgraphicsview.cpp",
        "mainwindow.cpp",
        "polygon.cpp",
        "ray.cpp",
    ],
    hdrs = [
        "controller.h",
        "customgraphicsview.h",
        "mainwindow.h",
        "polygon.h",
        "ray.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_gui",
    ],
)

qt_cc_binary(
    name = "raycaster",
    srcs = ["main.cpp"],
    deps = [
        ":raycaster_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_gui",
    ],
)

pkg_tar(
    name = "release",
    srcs = [":raycaster"],
    include_runfiles = True,
    package_dir = "app",
)