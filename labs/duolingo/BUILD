load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "duolingo_lib",
    srcs = [
        "mainwindow.cpp",
        "grammarexercise.cpp",
        "translationexercise.cpp",
    ],
    hdrs = [
        "mainwindow.h",
        "grammarexercise.h",
        "translationexercise.h",
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
        "@rules_qt//:qt_widgets",
    ],
    data = [
        "progress.json",
        "exercises.json",
    ],
)
