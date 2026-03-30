load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "controller_lib",
    srcs = ["Controller.cpp"],
    hdrs = ["Controller.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":paint_widget_lib",
        ":polygon_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_library(
    name = "paint_widget_lib",
    srcs = ["PaintWidget.cpp"],
    hdrs = ["PaintWidget.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":polygon_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

# Schedule library
qt_cc_library(
    name = "polygon_lib",
    srcs = ["Polygon.cpp"],
    hdrs = ["Polygon.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":ray_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

# Utils library
qt_cc_library(
    name = "ray_lib",
    srcs = ["Ray.cpp"],
    hdrs = ["Ray.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

# Main binary
qt_cc_binary(
    name = "raycaster",
    srcs = ["main.cpp"],
    deps = [
        ":controller_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)
