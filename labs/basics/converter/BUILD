load("@rules_qt//:qt.bzl", "qt_cc_library", "qt_cc_binary")

qt_cc_library(
    name = "converter_lib",
    srcs = [
        "mainwindow.cpp",
    ],
    hdrs = [
        "mainwindow.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "converter",
    srcs = ["main.cpp"],
    deps = [
        ":converter_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
