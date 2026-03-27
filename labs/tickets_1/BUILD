load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")
qt_cc_library(
    name = "tickets_lib",
    srcs = [
        "src/AddTicketsDialog.cpp",
        "src/CursorChangingButton.cpp",
        "src/EditableLabel.cpp",
        "src/InputUtils.cpp",
        "src/StatusComboBox.cpp",
        "src/TicketApp.cpp",
        "src/TicketCell.cpp",
        "src/TicketTableWidget.cpp",
        "src/TicketTextEdit.cpp",
        "src/TicketsProgressBar.cpp"
    ],
    hdrs = [
        "src/AddTicketsDialog.h",
        "src/CursorChangingButton.h",
        "src/EditableLabel.h",
        "src/InputUtils.h",
        "src/ProjectConstants.h",
        "src/StatusComboBox.h",
        "src/TicketApp.h",
        "src/TicketCell.h",
        "src/TicketsProgressBar.h",
        "src/TicketTableWidget.h",
        "src/TicketTextEdit.h"
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_gui",
    ],
)

qt_cc_binary(
    name = "tickets_1",
    srcs = ["src/main.cpp"],  # Исходный код для основной программы
    deps = [":tickets_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
	"@rules_qt//:qt_gui",
    ],
)
