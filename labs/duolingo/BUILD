# У меня были траблы с texttospeech я ее подключал через костыли LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu bazel run labs/duolingo
# Перевод очень кривой

load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

cc_library(
    name = "system_qt_tts",
    linkopts = ["-lQt6TextToSpeech"],
    visibility = ["//visibility:public"],
)

qt_cc_library(
    name = "duolingoLib",
    srcs = [
        "DifficultyDialog.cpp",
        "Exercises.cpp",
        "MainWindow.cpp",
        "TaskManager.cpp",
    ],
    hdrs = [
        "DifficultyDialog.h",
        "Exercises.h",
        "MainWindow.h",
        "TaskManager.h",
        "Utils.h",
    ],
    # Tell the compiler where the actual files are
    copts = [
        "-I/usr/include/x86_64-linux-gnu/qt6",
        "-I/usr/include/x86_64-linux-gnu/qt6/QtTextToSpeech",
    ],
    deps = [
        ":system_qt_tts",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "duolingo",
    srcs = ["main.cpp"],
    copts = [
        "-I/usr/include/x86_64-linux-gnu/qt6",
    ],
    data = ["tasks.json"],
    deps = [
        ":duolingoLib",
        ":system_qt_tts",
    ],
)
