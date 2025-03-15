load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_qt//:qt.bzl", "qt_cc_library", "qt_ui_library")

# Правило для обработки .ui файла
qt_ui_library(
    name = "mainwindow_ui",
    ui = "mainwindow.ui",
    deps = [
        "@rules_qt//:qt_widgets",
    ],
)

# Библиотека для твоего кода
qt_cc_library(
    name = "task2_lib",
    srcs = [
        "main.cpp",
        "mainwindow.cpp",
    ],
    hdrs = [
        "mainwindow.h",
    ],
    copts = [
        "-Iexternal/rules_qt",  # Путь к зависимостям rules_qt
        "-I$(GENDIR)/labs/basics/task2",  # Путь к сгенерированным файлам (bazel-bin/labs/basics/task1)
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":mainwindow_ui",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

# Исполняемый файл
cc_binary(
    name = "task2",
    srcs = ["main.cpp"],
    deps = [
        ":task2_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)
