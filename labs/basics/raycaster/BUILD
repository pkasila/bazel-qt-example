load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library", "qt_resource_via_qrc")


qt_resource_via_qrc(
    name = "resources",
    files = [
        "images/maxresdefault(1).png",
        "images/maxresdefault.png",
    ],
    qrc_file = "resource.qrc",
)

qt_cc_library(
    name = "Ray",
    srcs = ["Ray.cpp"],
    hdrs = ["Ray.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_opengl_widgets",
    ],
)


qt_cc_library(
    name = "Polygon",
    srcs = ["Polygon.cpp"],
    hdrs = ["Polygon.h"],
    deps = [
        ":Ray",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_opengl_widgets",
    ],
)

qt_cc_library(
    name = "Controller",
    srcs = ["Controller.cpp", ],
    hdrs = ["Controller.h", ],
    deps = [
        ":Polygon",
        ":Ray",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_opengl_widgets",
    ],
)

qt_cc_library(
    name = "Raycaster",
    srcs = [
       "Raycaster.cpp", 
    ],
    hdrs = ["Raycaster.h"],
    deps = [
        ":resources",
        ":Controller",
        ":Ray",
        ":Polygon",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_opengl_widgets",
    ],
)

qt_cc_binary(
    name = "raycaster",
    srcs = ["main.cpp"],
    deps = [
        ":Raycaster",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_opengl_widgets",
    ],
)
