load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

QT_COPTS = [
    "-std=c++17",
    "-fPIC",
    "-DQT_CORE_LIB",
    "-DQT_GUI_LIB",
    "-DQT_WIDGETS_LIB",
    "-I/usr/include/x86_64-linux-gnu/qt5",
    "-I/usr/include/x86_64-linux-gnu/qt5/QtCore",
    "-I/usr/include/x86_64-linux-gnu/qt5/QtGui",
    "-I/usr/include/x86_64-linux-gnu/qt5/QtWidgets",
]

QT_LINKOPTS = [
    "-L/usr/lib/x86_64-linux-gnu",
    "-lQt5Core",
    "-lQt5Gui",
    "-lQt5Widgets",
    "-lpthread",
    "-Wl,-rpath,/usr/lib/x86_64-linux-gnu",
]

genrule(
    name = "moc_mainwindow",
    srcs = ["mainwindow.h"],
    outs = ["moc_mainwindow.cpp"],
    cmd = "moc $(location mainwindow.h) -o $(location moc_mainwindow.cpp) " +
          "-I/usr/include/x86_64-linux-gnu/qt5 " +
          "-I/usr/include/x86_64-linux-gnu/qt5/QtCore " +
          "-I/usr/include/x86_64-linux-gnu/qt5/QtGui " +
          "-I/usr/include/x86_64-linux-gnu/qt5/QtWidgets",
)

genrule(
    name = "moc_ticketmanager",
    srcs = ["ticketmanager.h"],
    outs = ["moc_ticketmanager.cpp"],
    cmd = "moc $(location ticketmanager.h) -o $(location moc_ticketmanager.cpp) " +
          "-I/usr/include/x86_64-linux-gnu/qt5 " +
          "-I/usr/include/x86_64-linux-gnu/qt5/QtCore " +
          "-I/usr/include/x86_64-linux-gnu/qt5/QtGui " +
          "-I/usr/include/x86_64-linux-gnu/qt5/QtWidgets",
)

cc_library(
    name = "ticketmanager_lib",
    srcs = ["ticketmanager.cpp", ":moc_ticketmanager"],
    hdrs = ["ticketmanager.h"],
    copts = QT_COPTS,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "mainwindow_lib",
    srcs = ["mainwindow.cpp", ":moc_mainwindow"],
    hdrs = ["mainwindow.h"],
    copts = QT_COPTS,
    deps = [":ticketmanager_lib"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "task1",
    srcs = ["main.cpp"],
    copts = QT_COPTS,
    linkopts = QT_LINKOPTS,
    deps = [":mainwindow_lib", ":ticketmanager_lib"],
)
