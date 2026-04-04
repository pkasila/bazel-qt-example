load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "ray_lib",
    srcs = ["ray.cpp"],
    hdrs = ["ray.h"],
    visibility = ["//visibility:public"],
    copts = [
        "-I/usr/include/x86_64-linux-gnu/qt5",
        "-I/usr/include/x86_64-linux-gnu/qt5/QtCore",
    ],
)

cc_library(
    name = "polygon_lib",
    srcs = ["polygon.cpp"],
    hdrs = ["polygon.h"],
    visibility = ["//visibility:public"],
    deps = [":ray_lib"],
    copts = [
        "-I/usr/include/x86_64-linux-gnu/qt5",
        "-I/usr/include/x86_64-linux-gnu/qt5/QtCore",
    ],
)

cc_library(
    name = "controller_lib",
    srcs = ["controller.cpp"],
    hdrs = ["controller.h"],
    visibility = ["//visibility:public"],
    deps = [":ray_lib", ":polygon_lib"],
    copts = [
        "-I/usr/include/x86_64-linux-gnu/qt5",
        "-I/usr/include/x86_64-linux-gnu/qt5/QtCore",
    ],
)

cc_library(
    name = "gui_lib",
    srcs = ["main_window.cpp"],
    hdrs = ["main_window.h"],
    visibility = ["//visibility:public"],
    deps = [":controller_lib"],
    copts = [
        "-std=c++17",
        "-fPIC",
        "-I/usr/include/x86_64-linux-gnu/qt5",
        "-I/usr/include/x86_64-linux-gnu/qt5/QtCore",
        "-I/usr/include/x86_64-linux-gnu/qt5/QtWidgets",
        "-I/usr/include/x86_64-linux-gnu/qt5/QtGui",
    ],
    linkopts = [
        "-lQt5Core",
        "-lQt5Widgets",
        "-lQt5Gui",
    ],
)

cc_binary(
    name = "raycaster",
    srcs = [],
    deps = [":gui_lib"],
)
