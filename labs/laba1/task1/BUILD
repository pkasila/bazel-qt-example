load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_resource_via_qrc")

qt_resource_via_qrc(
    name = "cat_photos",
    qrc_file = "cats.qrc",
    files = [
        "assets/cat1.jpg",
        "assets/cat2.jpg",
        "assets/cat3.jpg",
        "assets/cat4.jpg",
        "assets/cat5.png",
        "assets/cat6.jpg",
    ],
)

qt_cc_binary(
    name = "laba1",
    srcs = [
        "main.cpp",
        "main_window.h",
        "main_window.cpp",
        "main_window_ui.inl",
        "main_window_logic.inl",
        "main_window_settings.inl",
    ],
    deps = [
        ":cat_photos",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
