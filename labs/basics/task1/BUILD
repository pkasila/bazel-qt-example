load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "ticket_app",
    srcs = [
        "ticketapp.cpp",
    ],
    hdrs = ["ticketapp.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "task1",
    srcs = ["main.cpp"],
    deps = [
        ":ticket_app",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
