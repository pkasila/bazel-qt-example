load("@rules_qt//:qt.bzl", "qt_cc_binary")

qt_cc_binary(
    name = "lab_1_1",
    srcs = [
        "main.cpp",
        "Header.h",
    ],
    copts = ["-std=c++17"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_gui",
    ],
)
