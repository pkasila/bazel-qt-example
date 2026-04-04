load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "custom_widgets",
    srcs = ["custom_widgets.cpp"],
    hdrs = ["custom_widgets.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_library(
    name = "main_window",
    srcs = ["main_window.cpp"],
    hdrs = ["main_window.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":custom_widgets",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_library(
    name = "resources",
    srcs = ["resources_rc.cpp"],
    hdrs = [],
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
        ":main_window",
        ":custom_widgets",
        ":resources",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)