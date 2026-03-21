load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

# Собираем библиотеку (тут автоматически прогоняется moc для Q_OBJECT)
qt_cc_library(
    name = "mainwindow",
    srcs = ["mainwindow.cpp"],
    hdrs =["mainwindow.h"],
    deps =[
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_gui",
    ],
)

# Собираем бинарник
qt_cc_binary(
    name = "lab1",
    srcs = ["main.cpp"],
    deps =[
        ":mainwindow",
        "@rules_qt//:qt_widgets",
    ],
)