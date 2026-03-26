load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library", "qt_resource")

qt_resource(
    name = "basics_res",
    files = [
        "qml/main.qml",
    ],
)

qt_cc_library(
    name = "basics_lib",
    srcs = [
    	"src/reviser.cpp",
    	"src/ticket.cpp",
    ],
    hdrs = [
    	"src/reviser.hpp",
    	"src/ticket.hpp",
    ],
    deps = [
    	":basics_res",
        "@rules_qt//:qt",
    ],
)

qt_cc_binary(
    name = "basics",
    srcs = [ "src/main.cpp" ],
    deps = [
    	":basics_res",
    	":basics_lib",
        "@rules_qt//:qt",
    ],
)
