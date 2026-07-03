load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")
qt_cc_library(
    name = "raycaster_lib",
    srcs = [
        "src/Controller.cpp",
        "src/DrawingArea.cpp",
        "src/GeometryUtils.cpp",
        "src/main.cpp",
        "src/MainWindow.cpp",
        "src/Polygon.cpp",
        "src/Ray.cpp",
        "src/Segment.cpp",
        "src/Vector2D.cpp"
    ],
    hdrs = [
        "src/Controller.h",
        "src/DrawingArea.h",
        "src/GeometryUtils.h",
        "src/MainWindow.h",
        "src/Mode.h",
        "src/Polygon.h",
        "src/ProjectConstants.h",
        "src/Ray.h",
        "src/Segment.h",
        "src/Vector2D.h"
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_gui",
    ],
)

qt_resource_via_qrc(
    name = "resources",
    files = [
	"icons/light.png",
	"icons/polygon.png",
    ],
    qrc_file = "resources.qrc",
)

qt_cc_binary(
    name = "raycaster",
    srcs = ["src/main.cpp",
            "resources.cpp"],
    data = ["resources.qrc",
            "icons/light.png",
	    "icons/polygon.png"],
    deps = [":raycaster_lib",
            "@rules_qt//:qt_core",
            "@rules_qt//:qt_widgets",
            "@rules_qt//:qt_gui",
    ],
)
