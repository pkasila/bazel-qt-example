load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "app_lib",
    srcs = [
        "greenhouse_control_app.cpp",
    ],
    hdrs = [
        "greenhouse_control_app.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "lab2_greenhouse",
    srcs = [
        "main.cpp",
    ],
    deps = [
        ":app_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)
