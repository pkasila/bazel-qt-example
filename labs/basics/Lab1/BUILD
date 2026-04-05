load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "ticket_review_app",
    srcs = [
        "ticket_review_app.cpp",
    ],
    hdrs = ["ticket_review_app.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "TicketReviewApp",
    srcs = ["main.cpp"],
    deps = [
        ":ticket_review_app",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
