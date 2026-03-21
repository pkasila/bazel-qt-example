load("@rules_qt6//:qt.bzl", "qt_cc_binary")

qt_cc_binary(
    name = "exam_app",
    srcs = [
        "main.cpp",
        "exam_app.cpp",
        "exam_app.h",
    ],
    deps = [
        "@rules_qt6//:qt_widgets",
        "@rules_qt6//:qt_core",
        "@rules_qt6//:qt_gui",
    ],
)
