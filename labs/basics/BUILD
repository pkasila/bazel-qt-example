load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "ticket_reviewer_lib",
    srcs = ["ticket_reviewer.cpp"],
    hdrs = ["ticket_reviewer.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "ticket_reviewer",
    srcs = ["ticket_reviewer_main.cpp"],
    deps = [
        ":ticket_reviewer_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_library(
    name = "color_mixer_lib",
    srcs = ["color_mixer.cpp"],
    hdrs = ["color_mixer.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "color_mixer",
    srcs = ["color_mixer_main.cpp"],
    deps = [
        ":color_mixer_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
