# labs/duolingo/BUILD.bazel
package(default_visibility = ["//visibility:public"])

genrule(
    name = "duolingo_moc",
    srcs = ["mainwindow.h"],
    outs = ["moc_mainwindow.cpp"],
    cmd = "/usr/lib/qt6/libexec/moc $(SRCS) -o $@",
)

cc_binary(
    name = "duolingo",
    srcs = [
        "main.cpp",
        "mainwindow.cpp",
        "mainwindow.h",
        ":duolingo_moc",
    ],
    copts = [
        "-std=c++17",
        "-fPIC",
        "-DQT_WIDGETS_LIB",
        "-DQT_GUI_LIB",
        "-DQT_CORE_LIB",
        "-DQT_MULTIMEDIA_LIB",  # ← Добавлено
        "-I/usr/include/x86_64-linux-gnu/qt6",
        "-I/usr/include/x86_64-linux-gnu/qt6/QtCore",
        "-I/usr/include/x86_64-linux-gnu/qt6/QtGui",
        "-I/usr/include/x86_64-linux-gnu/qt6/QtWidgets",
        "-I/usr/include/x86_64-linux-gnu/qt6/QtMultimedia",  # ← Добавлено
    ],
    linkopts = [
        "-L/usr/lib/x86_64-linux-gnu",
        "-lQt6Widgets",
        "-lQt6Gui",
        "-lQt6Core",
        "-lQt6Multimedia",  # ← Добавлено
        "-lpthread",
    ],
)