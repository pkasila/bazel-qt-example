load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "lang_app_lib",
    srcs = ["lang_app.cpp"],
    hdrs = ["lang_app.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "dualingo",
    srcs = ["main.cpp"],
    data = ["cat.png"],
    deps = [
        ":lang_app_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)