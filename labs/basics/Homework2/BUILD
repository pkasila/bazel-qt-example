load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "homework2_lib",
    srcs = [
        "src/app/DrawWidget.cpp",
        "src/app/MainWindow.cpp",
        "src/core/Controller.cpp",
        "src/core/Polygon.cpp",
        "src/core/Ray.cpp",
        "src/geometry/GeometryUtils.cpp",
    ],
    hdrs = [
        "src/app/DrawWidget.h",
        "src/app/MainWindow.h",
        "src/core/Controller.h",
        "src/core/Polygon.h",
        "src/core/Ray.h",
        "src/geometry/GeometryUtils.h",
        "src/ui/Modes.h",
        "src/ui/RenderConfig.h",
    ],
    includes = ["src"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "basics",
    srcs = ["src/main.cpp"],
    deps = [
        ":homework2_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
    visibility = ["//visibility:public"],
)