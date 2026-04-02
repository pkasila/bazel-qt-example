load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")


qt_cc_library(
    name = "main_window",
    srcs = ["mainwindow.cpp",
        "baseexercisewidget.cpp",
        "grammarexercisewidget.cpp",
        "difficultydialog.cpp",
        "exercisedata.cpp",
        "translationexercisewidget.cpp",
    ],
    hdrs = [ "mainwindow.h",
        "baseexercisewidget.h",
        "grammarexercisewidget.h",
        "difficultydialog.h",
        "exercisedata.h",
        "translationexercisewidget.h",
        "exercisestructures.h",
        ],
    deps = [
            "@rules_qt//:qt_core",
            "@rules_qt//:qt_gui",
            "@rules_qt//:qt_widgets",
    ],
)


qt_cc_binary(
    name = "english",
    srcs = ["main.cpp"],
    deps = [
    ":main_window",
    ],
)

