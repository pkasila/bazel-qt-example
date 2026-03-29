load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_binary(
    name = "basics_2",
    srcs = ["main.cpp"],
    data = [
        "google-6648-english.txt",  # Добавляем файл с данными
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)

load("@rules_pkg//pkg:tar.bzl", "pkg_tar")

pkg_tar(
    name = "release",
    srcs = [
        ":basics_2",
        "google-6648-english.txt",  # Добавляем файл в архив
    ],
    include_runfiles = True,
    package_dir = "app",
)
