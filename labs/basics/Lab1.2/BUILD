load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "mood_journal_lib",
    srcs = [
        "mood_journal.cpp",
    ],
    hdrs = ["mood_journal.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_gui",
    ],
)

qt_cc_binary(
    name = "mood_journal",
    srcs = ["main.cpp"],
    deps = [
        ":mood_journal_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
