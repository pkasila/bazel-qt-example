# BUILD

load("@rules_qt//qt:qt.bzl", "qt_cc_binary", "qt_cc_library")
load("@rules_qt//qt:qt.bzl", "qt_tool_cc_library") # Needed for moc processing

# Define a base library for common Qt modules
package(default_visibility = ["//visibility:public"]) # Make libraries visible to the binary

qt_tool_cc_library(
    name = "language_app_base_lib",
    srcs = [], # No source files, just defines common dependencies
    hdrs = [],
    qt_modules = [
        "Core",
        "Gui",
        "Widgets",
        "Multimedia", # For potential audio bonus
        "MultimediaWidgets", # For potential audio bonus UI
    ],
)

# Define libraries for each component
qt_cc_library(
    name = "main_window_lib",
    srcs = ["src/main_window.cpp"],
    hdrs = ["src/main_window.h"],
    deps = [
        ":language_app_base_lib",
        "//src/ui:main_menu_widget_lib",
        "//src/ui:translation_exercise_widget_lib",
        "//src/ui:grammar_exercise_widget_lib",
        "//src/dialogs:difficulty_dialog_lib",
        "//src/core:exercise_manager_lib",
    ],
)

qt_cc_library(
    name = "main_menu_widget_lib",
    srcs = ["src/ui/main_menu_widget.cpp"],
    hdrs = ["src/ui/main_menu_widget.h"],
    deps = [":language_app_base_lib"],
)

qt_cc_library(
    name = "translation_exercise_widget_lib",
    srcs = ["src/ui/translation_exercise_widget.cpp"],
    hdrs = ["src/ui/translation_exercise_widget.h"],
    deps = [":language_app_base_lib"],
)

qt_cc_library(
    name = "grammar_exercise_widget_lib",
    srcs = ["src/ui/grammar_exercise_widget.cpp"],
    hdrs = ["src/ui/grammar_exercise_widget.h"],
    deps = [":language_app_base_lib"],
)

qt_cc_library(
    name = "difficulty_dialog_lib",
    srcs = ["src/dialogs/difficulty_dialog.cpp"],
    hdrs = ["src/dialogs/difficulty_dialog.h"],
    deps = [
        ":language_app_base_lib",
        "//src/core:exercise_data_lib", # Needs access to difficulty levels
    ],
)

cc_library( # cc_library because ExerciseData doesn't use Q_OBJECT
    name = "exercise_data_lib",
    srcs = [], # Data might be defined directly in the header
    hdrs = ["src/core/exercise_data.h"],
    deps = [":language_app_base_lib"], # QMap, QString
)

qt_cc_library(
    name = "exercise_manager_lib",
    srcs = ["src/core/exercise_manager.cpp"],
    hdrs = ["src/core/exercise_manager.h"],
    deps = [
        ":language_app_base_lib",
        ":exercise_data_lib",
        "//src/utils:string_compare_lib",
    ],
)

cc_library( # cc_library because string_compare doesn't use Q_OBJECT
    name = "string_compare_lib",
    srcs = ["src/utils/string_compare.cpp"],
    hdrs = ["src/utils/string_compare.h"],
    deps = [":language_app_base_lib"], # QString, QRegularExpression
)


# Define the main executable
qt_cc_binary(
    name = "language_app",
    srcs = ["src/main.cpp"],
    deps = [
        ":main_window_lib",
        ":language_app_base_lib", # QCoreApplication
    ],
)