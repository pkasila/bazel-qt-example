load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")
qt_cc_library(
    name = "medicine_lib",
    srcs = [
        "src/AddMedicineDialog.cpp",
	"src/MedicineApp.cpp"
    ],
    hdrs = [
        "src/AddMedicineDialog.h",
	"src/Medicine.h",
	"src/MedicineApp.h",
	"src/ProjectConstants.h"
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_gui",
    ],
)

qt_cc_binary(
    name = "medicine_app",
    srcs = ["src/main.cpp"],
    deps = [":medicine_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_gui",
    ],
)
