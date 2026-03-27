load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library", "qt_resource_via_qrc")

qt_resource_via_qrc(
    name = "resources",
    files = [
        "images/australia2024.png",
        "images/f1_background.png",
        "images/alpine.jpg",
        "images/haas.jpg",
        "images/racing_bulls.jpg",
        "images/willams.jpg",
        "images/sauber.jpg",
        "images/aston_martin.jpg",
        "images/mercedes.jpg",
        "images/red_bull.jpg",
        "images/ferrari.jpg",
        "images/mclaren.jpg",
        "images/spa.jpeg",
        "images/monza.jpeg",
        "images/montreal.jpeg",
        "images/monaco.jpeg",
        "images/f1_logo.png",
        "images/left_spa.png",
        "images/left_monza.png",
        "images/left_montr.png",
        "images/left_monaco.png",
        "images/left_melb.png",
        "images/sauber_l.jpg",
        "images/willams_l.jpg",
        "images/haas_l.png",
        "images/racing_bulls_l.jpg",
        "images/alpine_l.jpg",
        "images/aston_martin_l.jpeg",
        "images/ferrari_l.jpg",
        "images/red_bull_l.jpg",
        "images/mclaren_l.jpg",
        "images/mercedes_l.jpg"
    ],
    qrc_file = "resources.qrc",
)

qt_cc_library(
    name = "mainwindow",
    srcs = ["mainwindow.cpp"],
    hdrs = ["mainwindow.h"],
    deps = [
        ":resources",
        ":merchwindow",
        ":ticketwindow",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_library(
    name = "merchwindow",
    srcs = ["merchwindow.cpp"],
    hdrs = ["merchwindow.h"],
    deps = [
        ":resources",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_library(
    name = "ticketwindow",
    srcs = ["ticketwindow.cpp"],
    hdrs = ["ticketwindow.h"],
    deps = [
        ":resources",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        ":mainwindow",
        ":merchwindow",
        ":ticketwindow",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)
