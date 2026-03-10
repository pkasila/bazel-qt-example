load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library", "qt_resource_via_qrc")


qt_resource_via_qrc(
    name = "resources",
    files = [
        "sounds/success.wav",
        "sounds/failed.wav",
    ],
    qrc_file = "resources.qrc",
)

qt_cc_library(
    name = "language_learning_lib",
    srcs = [
        "main_window.cpp",
    ],
    hdrs = [
        "main_window.h", "exercise.h",
    ],
    deps = [
        ":resources",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_multimedia",
    ],
    linkopts = ["-Wl,--allow-multiple-definition"],
)

qt_cc_binary(
    name = "duolingo",
    srcs = [
        "main.cpp",
    ],
    deps = [
        ":resources",
        ":language_learning_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_multimedia",
    ],
    linkopts = ["-Wl,--allow-multiple-definition"],
) 