load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "1984_lib",
    srcs = ["mainwindow.cpp","board.cpp","ray.cpp","polygon.cpp","controller.cpp"],
    hdrs = ["mainwindow.h","board.h","ray.h","polygon.h","controller.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "1984",
    srcs = ["main.cpp",],
    deps = [
        ":1984_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)

load("@rules_pkg//pkg:tar.bzl", "pkg_tar")

pkg_tar(
    name = "release",
    srcs = [
        ":1984",
    ],
    include_runfiles = True,
    package_dir = "1984",
)
