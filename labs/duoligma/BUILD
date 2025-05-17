load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library", "qt_resource")

qt_resource(
    name = "resources",
    alias_relative = True,
    files = glob(["resources/**/*"]),
)

qt_cc_library(
    name = "mainwindow",
    srcs = ["mainwindow.cpp"],
    hdrs = ["mainwindow.h"],
    deps = [
        ":resources",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_multimedia",
        "@rules_qt//:qt_sql",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "duoligma",
    srcs = ["main.cpp"],
    deps = [
        ":mainwindow",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
