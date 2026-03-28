load("@rules_qt//:qt.bzl", "qt_cc_library", "qt_cc_binary")

qt_cc_library(
    name = "procrastination_lib",
    srcs = [
        "infobox.cpp",
        "mainwindow.cpp",
        "ticketitem.cpp",
        "ticketlist.cpp",
    ],
    hdrs = [
        "infobox.h",
        "mainwindow.h",
        "ticketitem.h",
        "ticketlist.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "procrastination",
    srcs = ["main.cpp"],
    deps = [
        ":procrastination_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)

load("@rules_pkg//pkg:tar.bzl", "pkg_tar")

pkg_tar(
    name = "release",
    srcs = [":procrastination"],
    include_runfiles = True,
    package_dir = "app",
)
