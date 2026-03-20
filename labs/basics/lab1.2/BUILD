load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "ocean_control_app_lib",
    srcs = ["ocean_control_app.cpp"],
    hdrs = ["ocean_control_app.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "free_swim_lab",
    srcs = ["main.cpp"],
    deps = [
        ":ocean_control_app_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)
