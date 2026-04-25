load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "duolingo_lib",
    srcs = [
        "mainwindow.cpp",
        "difficulty_dialog.cpp",
        "exercise_session.cpp",
        "string_utils.cpp",
    ],
    hdrs = [
        "mainwindow.h",
        "difficulty_dialog.h",
        "exercise_session.h",
        "string_utils.h",
        "task.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "duolingo",
    srcs = ["main.cpp"],
    deps = [
        ":duolingo_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)