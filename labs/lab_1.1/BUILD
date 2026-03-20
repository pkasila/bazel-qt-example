load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "app_library",
    srcs = ["main.cpp"],
    hdrs = ["Header.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_gui",
    ],
)

qt_cc_binary(
    name = "app",
    srcs = ["main.cpp"],
    deps = [
        ":app_library",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_gui",
    ],
)