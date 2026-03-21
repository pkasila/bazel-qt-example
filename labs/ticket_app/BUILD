load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "ticket_app_lib",
    srcs = ["ticket_app.cpp"],
    hdrs = ["ticket_app.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "ticket_app",
    srcs = ["main.cpp"],
    deps = [
        ":ticket_app_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)