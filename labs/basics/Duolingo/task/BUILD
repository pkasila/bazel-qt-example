load("@rules_pkg//pkg:tar.bzl", "pkg_tar")
load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "database",
    srcs = [
        "database/database_manager.cpp",
    ],
    hdrs = [
        "database/database_manager.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_sql",
    ],
)

qt_cc_library(
    name = "languagetool",
    srcs = [
        "languagetool/languagetool_client.cpp",
        "languagetool/answer_evaluator.cpp",
    ],
    hdrs = [
        "languagetool/languagetool_client.h",
        "languagetool/answer_evaluator.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_network",
    ],
)

qt_cc_library(
    name = "interface",
    srcs = [
        "interface/mainwindow.cpp",
        "interface/lesson_widget.cpp",
        "interface/sidebar_widget.cpp",
        "interface/translation_widget.cpp",
        "interface/grammar_widget.cpp",
        "interface/difficulty_dialog.cpp",
        "interface/admin_dialog.cpp",
        "interface/lt_settings_dialog.cpp",
    ],
    hdrs = [
        "interface/mainwindow.h",
        "interface/lesson_widget.h",
        "interface/sidebar_widget.h",
        "interface/translation_widget.h",
        "interface/grammar_widget.h",
        "interface/difficulty_dialog.h",
        "interface/admin_dialog.h",
        "interface/lt_settings_dialog.h",
        "interface/styles.h",
        "interface/utils.h",
    ],
    deps = [
        ":database",
        ":languagetool",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_network",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "app",
    srcs = [
        "main.cpp",
    ],
    deps = [
        ":interface",
        ":database",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "languagetool_test",
    srcs = [
        "languagetool/languagetool_test.cpp",
    ],
    deps = [
        ":languagetool",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_network",
    ],
)

pkg_tar(
    name = "release",
    srcs = [
        ":app",
    ],
    include_runfiles = True,
    package_dir = "app",
)
