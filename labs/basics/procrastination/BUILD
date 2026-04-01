load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "procrastination_lib",
    srcs = [
        "procrastination.cpp",
    ],
    hdrs = ["procrastination.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
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
