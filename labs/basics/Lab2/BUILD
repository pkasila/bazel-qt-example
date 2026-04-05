load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "factorizer",
    srcs = [
        "factorizer.cpp",
    ],
    hdrs = ["factorizer.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "Factorizer",
    srcs = ["main.cpp"],
    deps = [
        ":factorizer",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
