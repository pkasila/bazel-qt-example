load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library", "qt_resource_via_qrc")

qt_cc_library(
    name = "main_window",
    srcs = [
        "grammar.cpp",
        "leftmenu.cpp",
        "mainwindow.cpp",
        "translate.cpp"
    ],
    hdrs = [
        "grammar.h",
        "leftmenu.h",
        "mainwindow.h",
        "translate.h"
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_multimedia"
    ],
)

qt_resource_via_qrc(
    name = "app_resources",
    qrc_file = "source.qrc",
    files = [
        "audio/correct_ans.wav",
        "audio/bad_answer.wav",
        "audio/level_complete.wav",
        "audio/lives_lost.wav",
        "audio/click.wav"
    ],
)

qt_cc_binary(
    name = "lab3",
    srcs = ["main.cpp"],
    deps = [
        ":main_window",
        ":app_resources",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets"
    ],
)