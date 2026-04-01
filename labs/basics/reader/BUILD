load(
    "@rules_qt//:qt.bzl",
    "qt_cc_binary",
    "qt_cc_library",
    "qt_resource_via_qrc",
)

qt_resource_via_qrc(
    name = "resources",
    files = [
        "resources/books/mushoku-tensei/chapters/c1.txt",
        "resources/books/mushoku-tensei/chapters/c2.txt",
        "resources/books/mushoku-tensei/chapters/c3.txt",
        "resources/books/mushoku-tensei/titleInfo.json",
        "resources/books/omniscient-readers-viewpoint/chapters/c1.txt",
        "resources/books/omniscient-readers-viewpoint/chapters/c2.txt",
        "resources/books/omniscient-readers-viewpoint/chapters/c3.txt",
        "resources/books/omniscient-readers-viewpoint/titleInfo.json",
        "resources/books/sss-class-suicide-hunter/chapters/c1.txt",
        "resources/books/sss-class-suicide-hunter/chapters/c2.txt",
        "resources/books/sss-class-suicide-hunter/chapters/c3.txt",
        "resources/books/sss-class-suicide-hunter/titleInfo.json",
        "resources/cfg.json",
        "resources/images/mushoku-tensei.jpg",
        "resources/images/omniscient-readers-viewpoint.jpg",
        "resources/images/sss-class-suicide-hunter.jpg",
    ],
    qrc_file = ".qrc",
)

qt_cc_library(
    name = "reader_lib",
    srcs = [
        "src/ChapterView.cpp",
        "src/JsonWork.cpp",
        "src/RanobeList.cpp",
        "src/RanobeListItem.cpp",
        "src/RanobeReader.cpp",
        "src/RanobeView.cpp",
    ],
    hdrs = [
        "src/hdrs/ChapterView.h",
        "src/hdrs/JsonWork.h",
        "src/hdrs/RanobeList.h",
        "src/hdrs/RanobeListItem.h",
        "src/hdrs/RanobeReader.h",
        "src/hdrs/RanobeView.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "reader",
    srcs = ["main.cpp"],
    deps = [
        ":reader_lib",
        ":resources",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)
