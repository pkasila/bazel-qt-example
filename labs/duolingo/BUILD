load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "libs",
    srcs = [
        "exercise_widget.cpp",
        "grammar_exercise.cpp",
        "main_window.cpp",
        "translation_exercise.cpp",
    ],
    hdrs = [
        "exercise_widget.h",
        "grammar_exercise.h",
        "main_window.h",
        "translation_exercise.h",
    ],
    data = [
        "data/complete.wav",
        "data/correct.wav",
        "data/questions.db",
        "data/wrong.wav",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_multimedia",
        "@rules_qt//:qt_sql",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "duolingo",
    srcs = ["main.cpp"],
    deps = [
        ":libs",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
