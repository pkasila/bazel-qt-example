load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "main_window",
    srcs = [
        "calculator_controller.cpp",
        "main_window.cpp",
    ],
    hdrs = [
        "calculator_controller.h",
        "main_window.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "basics_1",
    srcs = ["main.cpp"],
    data = ["settings.txt"],
    deps = [
        ":main_window",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
