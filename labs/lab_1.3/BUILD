load("@rules_qt//:qt.bzl", "qt_cc_binary")

qt_cc_binary(
    name = "lab_1_3",
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
    data = [
        "1.0.jpg",
        "1.1.jpg",
        "1.2.jpg",
        "1.8.jpg",
    ],
)
