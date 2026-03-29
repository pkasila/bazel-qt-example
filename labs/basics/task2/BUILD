load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "library_app",
    srcs = [
        "book.cpp",
        "library.cpp",
        "user.cpp",
        "mainwindow.cpp",
    ],
    hdrs = [
    "book.h",
    "library.h",
    "user.h",
    "mainwindow.h",

    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "task2",
    srcs = ["main.cpp"],
    deps = [
        ":library_app",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
