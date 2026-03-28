load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "main_window",
    srcs = ["mainwindow.cpp", "grammartask.cpp", "translationtask.cpp"],
    hdrs = ["mainwindow.h", "grammartask.h", "translationtask.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_sql",
    ],
)

qt_cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    data = ["logo.png"],
    deps = [
        ":main_window",
        "@rules_qt//:qt_widgets",
    ],
)