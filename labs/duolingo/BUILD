load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library", "qt_resource", "qt_resource_via_qrc", "qt_ui_library")


qt_cc_library(
    name = "mainwindow",
    srcs = [
            "audioexercisewidget.cpp",
            "authdialog.cpp",
            "datamanager.cpp",
            "mainwindow.cpp",
            "userstatsdialog.cpp"
             ],
    hdrs = [
        "audioexercisewidget.h",
        "authdialog.h",
        "datamanager.h",
        "exercisetypes.h",
        "leveldata.h",
        "mainwindow.h",
        "user.h",
        "userstatsdialog.h"
    ],
#defines = ["QT_NO_KEYWORDS"],

    deps = [
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_multimedia",
        "@rules_qt//:qt_sql"
    ],
)
qt_resource_via_qrc(
    name = "resource2",
    files = [
            "styles/duolingo_style.qss",
            "audio/dog.mp3",
            "audio/cat.mp3",
            "audio/man.mp3",
            "audio/water.mp3",
            "audio/woman.mp3",
            "audio/blue.mp3",
            "audio/mother.mp3",
            "audio/sister.mp3",
            "audio/milk.mp3",
            "audio/cheese.mp3",
            "audio/cow.mp3",
            "audio/one.mp3",
            "audio/run.mp3",
            "audio/pencil.mp3",
            "audio/monday.mp3",
            "audio/sunday.mp3",
            "audio/february.mp3",
            "audio/may.mp3",
            "audio/doctor.mp3",
            "audio/sunny.mp3",
            "audio/trousers.mp3",
            "audio/how_much.mp3",
            "audio/five.mp3"
    ],
    qrc_file = "resources.qrc",
)
qt_cc_binary(
    name = "duolingo",
    srcs = ["main.cpp"],
        #defines = ["QT_NO_KEYWORDS"],

    deps = [
        ":mainwindow",
        ":resource2",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)