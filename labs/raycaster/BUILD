load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_binary(
    name = "raycaster",
    srcs = ["main.cpp"],
    copts = ["-std=c++17"],
    deps = [
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_gui", 
        "@rules_qt//:qt_core", 
    ],
)
