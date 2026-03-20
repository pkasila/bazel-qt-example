load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "ticket_manager_lib",
    srcs = ["ticket_manager.cpp"],
    hdrs = ["ticket_manager.h"],
    deps = [
        "@rules_qt//:qt_core",
    ],
)

qt_cc_library(
    name = "ticket_review_app_lib",
    srcs = ["ticket_review_app.cpp"],
    hdrs = ["ticket_review_app.h"],
    deps = [
        ":ticket_manager_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "lab1",
    srcs = ["main.cpp"],
    deps = [
        ":ticket_review_app_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)
