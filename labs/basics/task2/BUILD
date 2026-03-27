load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library", "qt_resource", "qt_resource_via_qrc", "qt_ui_library")


qt_cc_library(
    name = "main_window",
    srcs = [ "dummy.cpp" ],
    hdrs = [
        "Dialog.h",
        "DrawingTable.h",
        "PictureView.h",
        "GoodDelegate.h",
        "MyWindow.h",
    ],
defines = ["QT_NO_KEYWORDS"],

    deps = [
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
    ],
)
qt_resource_via_qrc(
    name = "resource2",
    files = [
        "p2.jpeg"
    ],
    qrc_file = "resources.qrc",
)
qt_cc_binary(
    name = "mainwindow",
    srcs = ["main.cpp"],
        defines = ["QT_NO_KEYWORDS"],

    deps = [
        ":main_window",
        ":resource2",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)