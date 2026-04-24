load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

QT_BASE = "/usr/include/x86_64-linux-gnu/qt5"

QT_LIB_PATH = "/usr/lib/x86_64-linux-gnu"

QT_MODULES = [
    "QtCore",
    "QtWidgets",
    "QtGui",
]

cc_library(
    name = "lingo_lib",
    srcs = [
        "difficulty_dialog.cpp",
        "exercise_data.cpp",
        "exercise_widgets.cpp",
        "main_window.cpp",
    ],
    hdrs = [
        "difficulty_dialog.h",
        "exercise_data.h",
        "exercise_widgets.h",
        "main_window.h",
    ],
    copts = [
        "-std=c++17",
        "-fPIC",
        "-Wall",
        "-Wextra",
        "-finput-charset=UTF-8",
        "-fexec-charset=UTF-8",
        "-I" + QT_BASE,
    ] + ["-I" + QT_BASE + "/" + mod for mod in QT_MODULES],
    linkopts = [
        "-L" + QT_LIB_PATH,
        "-lQt5Core",
        "-lQt5Widgets",
        "-lQt5Gui",
        "-Wl,-rpath," + QT_LIB_PATH,
    ],
    visibility = [":__subpackages__"],
)

cc_binary(
    name = "lingo_app",
    srcs = ["main.cpp"],
    copts = [
        "-std=c++17",
        "-fPIC",
        "-I" + QT_BASE,
    ] + ["-I" + QT_BASE + "/" + mod for mod in QT_MODULES],
    linkopts = [
        "-L" + QT_LIB_PATH,
        "-lQt5Core",
        "-lQt5Widgets",
        "-lQt5Gui",
        "-Wl,-rpath," + QT_LIB_PATH,
    ],
    visibility = ["//visibility:public"],
    deps = [":lingo_lib"],
)
