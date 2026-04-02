load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "library",
    srcs = [
        "mainwindow.cpp",
        "vectors.cpp",
    ],
    hdrs = ["mainwindow.h",
    "vectors.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_sql"
    ],
)

qt_cc_binary(
    name = "dudulingo",
    srcs = ["main.cpp"],
    deps = [
        ":library",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
