load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "task_2_lib",
    srcs = [
        "text_editor.cpp",
    ],
    hdrs = ["text_editor.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "task_2",
    srcs = ["main.cpp"],
    deps = [
        ":task_2_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
