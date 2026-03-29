load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library", "qt_resource")

qt_resource(
    name = "resources",
    files = glob([ "resources/**/*" ]),
    alias_relative = True,
)

qt_cc_library(
    name = "mainwindow",
    srcs = [ "mainwindow.cpp", ],
    hdrs = [ "mainwindow.h", "data.h" ],
    deps = [
        ":resources",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "questionnaire",
    srcs = ["main.cpp"],
    deps = [
        ":mainwindow",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
