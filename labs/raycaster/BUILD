load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "main_window",
    srcs = [
        "src/main_window.cpp",
    ],
    hdrs = [
        "src/main_window.h",
    ],
    deps = [
        ":drawing_widget",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_library(
    name = "ray",
    srcs = [
        "src/ray.cpp",
    ],
    hdrs = [
        "src/ray.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_library(
    name = "polygon",
    srcs = [
        "src/polygon.cpp",
    ],
    hdrs = [
        "src/polygon.h",
    ],
    deps = [
        ":ray",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_library(
    name = "controller",
    srcs = [
        "src/controller.cpp",
    ],
    hdrs = [
        "src/controller.h",
    ],
    deps = [
        ":polygon",
        ":ray",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_library(
    name = "drawing_widget",
    srcs = [
        "src/drawing_widget.cpp",
    ],
    hdrs = [
        "src/drawing_widget.h",
    ],
    deps = [
        ":controller",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        #        "@rules_qt//:qt_opengl",
        #        "@rules_qt//:qt_opengl_widgets",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "raycaster",
    srcs = ["main.cpp"],
    deps = [
        ":main_window",
        ":polygon",
        ":ray",
        "@rules_qt//:qt_core",
        #        "@rules_qt//:qt_opengl",
        #        "@rules_qt//:qt_opengl_widgets",
        "@rules_qt//:qt_widgets",
    ],
)
