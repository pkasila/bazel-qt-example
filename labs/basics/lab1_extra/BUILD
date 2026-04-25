load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "lighting_controller_lib",
    srcs = ["lighting_controller.cpp"],
    hdrs = ["lighting_controller.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "lab1_extra",
    srcs = ["main.cpp"],
    deps = [
        ":lighting_controller_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
