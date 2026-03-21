load("@rules_qt//:qt.bzl", "qt_cc_binary")

qt_cc_binary(
    name = "exam_app",
    srcs = [
        "main.cpp",
        "exam_app.cpp",
        "exam_app.h",
    ],
    deps = [
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
    ],
)
