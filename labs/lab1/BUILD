load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "lab_lib",
    srcs = [
        "mainwindow.cpp",
        "spaceapp.cpp",
    ],
    hdrs = [
        "mainwindow.h",
        "spaceapp.h",
        "launcher.h",
    ],
    deps = [
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_core",
    ],
)

qt_cc_binary(
    name = "lab1",
    srcs = ["main.cpp"],
    deps = [
        ":lab_lib",
    ],
)