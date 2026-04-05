load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

# Собираем всю логику в библиотеку
qt_cc_library(
    name = "raycaster_lib",
    srcs = [
        "mainwindow.cpp",
        "render_widget.cpp",
        "controller.cpp",
    ],
    hdrs = [
        "mainwindow.h",
        "render_widget.h",
        "math.h",
        "controller.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

# Бинарник только запускает приложение
qt_cc_binary(
    name = "raycaster_app",
    srcs = ["main.cpp"],
    deps = [
        ":raycaster_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)