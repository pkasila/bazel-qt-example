load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

filegroup(
    name = "data_files",
    srcs = glob(["data/**"]),
)


qt_cc_library(
    name = "mainwindow",
    srcs = [
        "mainwindow.cpp",
        "translationexercise.cpp",
        "grammarexercise.cpp",
        "settingsdialog.cpp",
        "multicorrectgrammarexercise.cpp",
    ],
    hdrs = [
        "mainwindow.h",
        "translationexercise.h",
        "grammarexercise.h",
        "settingsdialog.h",
        "exercisedata.h",
        "multicorrectgrammarexercise.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_multimedia",
    ],
    data = [":data_files"],
)

qt_cc_binary(
    name = "duolingo",
    srcs = ["main.cpp"],
    deps = [
        ":mainwindow",
    ],
)
