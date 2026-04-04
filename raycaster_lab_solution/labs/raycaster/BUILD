load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

QT_DEPS = select({
    "@platforms//os:linux": [
        "@qt_linux_x86_64//:QtCore",
        "@qt_linux_x86_64//:QtGui",
        "@qt_linux_x86_64//:QtWidgets",
    ],
    "@platforms//os:windows": [
        "@qt_windows_x86_64//:QtCore",
        "@qt_windows_x86_64//:QtGui",
        "@qt_windows_x86_64//:QtWidgets",
    ],
    "@rules_qt//:osx_x86_64": [
        "@qt_mac_x86_64//:QtCore",
        "@qt_mac_x86_64//:QtGui",
        "@qt_mac_x86_64//:QtWidgets",
    ],
    "@rules_qt//:osx_arm64": [
        "@qt_mac_aarch64//:QtCore",
        "@qt_mac_aarch64//:QtGui",
        "@qt_mac_aarch64//:QtWidgets",
    ],
})

qt_cc_library(
    name = "raycaster_lib",
    srcs = [
        "controller.cpp",
        "drawing_widget.cpp",
        "main_window.cpp",
        "polygon.cpp",
        "ray.cpp",
    ],
    hdrs = [],
    normal_hdrs = [
        "controller.h",
        "drawing_widget.h",
        "geometry_utils.h",
        "main_window.h",
        "polygon.h",
        "ray.h",
    ],
    deps = QT_DEPS,
    visibility = ["//visibility:public"],
)

qt_cc_binary(
    name = "basics",
    srcs = ["main.cpp"],
    deps = [
        ":raycaster_lib",
    ],
    visibility = ["//visibility:public"],
)

alias(
    name = "raycaster",
    actual = ":basics",
    visibility = ["//visibility:public"],
)
