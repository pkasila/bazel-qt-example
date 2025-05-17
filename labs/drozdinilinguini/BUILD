load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "drozdinilinguini_lib",
    srcs = [
        "mainwindow.cpp",
        "translate.cpp",
        "grammar.cpp",
    ],
    hdrs = [
        "mainwindow.h",
        "custommenu.h",
        "translate.h",
        "grammar.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_sql",
    ],
)

qt_cc_binary(
    name = "drozdinilinguini",
    srcs = ["main.cpp"],
    deps = [
        ":drozdinilinguini_lib",
        "@rules_qt//:qt_widgets",
    ],
    
)