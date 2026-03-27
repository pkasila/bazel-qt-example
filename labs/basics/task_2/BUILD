load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library", "qt_resource_via_qrc")

qt_resource_via_qrc(
    name = "resources",
    files = [
        "schedule.json",
    ],
    qrc_file = "resources.qrc",
)

# DateSchedule library
qt_cc_library(
    name = "date_schedule_lib",
    srcs = ["DateSchedule.cpp"],
    hdrs = ["DateSchedule.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":event_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

# Event library
qt_cc_library(
    name = "event_lib",
    srcs = ["Event.cpp"],
    hdrs = ["Event.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

# Schedule library
qt_cc_library(
    name = "schedule_lib",
    srcs = ["Schedule.cpp"],
    hdrs = ["Schedule.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":date_schedule_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

# Utils library
qt_cc_library(
    name = "utils_lib",
    srcs = ["Utils.cpp"],
    hdrs = ["Utils.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":schedule_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

# MainWindow library
qt_cc_library(
    name = "main_window_lib",
    srcs = ["main_window.cpp"],
    hdrs = ["main_window.h"],
    deps = [
        ":event_lib",
        ":resources",
        ":schedule_lib",
        ":utils_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

# Main binary
qt_cc_binary(
    name = "task_2",
    srcs = ["main.cpp"],
    deps = [
        ":main_window_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)
