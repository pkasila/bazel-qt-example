load("@rules_qt//:qt.bzl", "update_dict", "qt_plugin_data", "LINUX_ENV_DATA", "MAC_X64_ENV_DATA", "MAC_M1_ENV_DATA", "WINDOWS_ENV_DATA")
load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")



def qt1_cc_library(name, srcs, hdrs, normal_hdrs = [], deps = None, copts = [], target_compatible_with = [], **kwargs):
    """Compiles a QT library and generates the MOC for it.

    Args:
      name: A name for the rule.
      srcs: The cpp files to compile.
      hdrs: The header files that the MOC compiles to src.
      normal_hdrs: Headers which are not sources for generated code.
      deps: cc_library dependencies for the library.
      copts: cc_library copts
      target_compatible_with: A list of constraint_values that must be satisfied by the target
        platform in order for this toolchain to be selected for a target building for that platform.
      **kwargs: Any additional arguments are passed to the cc_library rule.
    """
    _moc_srcs = []
    for hdr in hdrs:
        header_path = "%s/%s" % (native.package_name(), hdr) if len(native.package_name()) > 0 else hdr
        moc_name = "moc1_%s" % hdr.rsplit(".", 1)[0]
        native.genrule(
            name = moc_name,
            srcs = [hdr],
            outs = [moc_name + ".cpp"],
            cmd = select({
                "@platforms//os:linux": "$(location @qt_linux_x86_64//:moc) $(locations %s) -o $@ -f'%s'" % (hdr, header_path),
                "@platforms//os:windows": "$(location @qt_windows_x86_64//:moc) $(locations %s) -o $@ -f'%s'" % (hdr, header_path),
                "@rules_qt//:osx_x86_64": "$(location @qt_mac_x86_64//:moc) $(locations %s) -o $@ -f'%s'" % (hdr, header_path),
                "@rules_qt//:osx_arm64": "$(location @qt_mac_aarch64//:moc) $(locations %s) -o $@ -f'%s'" % (hdr, header_path),
            }),
            tools = select({
                "@platforms//os:linux": ["@qt_linux_x86_64//:moc"],
                "@platforms//os:windows": ["@qt_windows_x86_64//:moc"],
                "@rules_qt//:osx_arm64": ["@qt_mac_aarch64//:moc"],
                "@rules_qt//:osx_x86_64": ["@qt_mac_x86_64//:moc"],
            }),
            target_compatible_with = target_compatible_with,
        )
        _moc_srcs.append(":" + moc_name)
    cc_library(
        name = name,
        srcs = srcs + _moc_srcs,
        hdrs = hdrs + normal_hdrs,
        textual_hdrs = _moc_srcs,
        deps = deps,
        copts = copts + select({
            "@platforms//os:windows": [],
            "//conditions:default": ["-fPIC"],
        }),
        target_compatible_with = target_compatible_with,
        **kwargs
    )



def qt1_cc_binary(name, srcs, deps = None, copts = [], data = [], env = {}, **kwargs):
    """ cc_binary which depend on qt_cc_library or want to use qt tools

    Args:
      name: A name for the rule.
      srcs: The cpp files to compile.
      deps: cc_library dependencies for the library.
      copts: cc_library copts
      data: which data need to depend
      env: environment value
      **kwargs: Any additional arguments are passed to the cc_library rule.
    """
    linux_env_data = update_dict(LINUX_ENV_DATA, env)
    mac_x64_env_data = update_dict(MAC_X64_ENV_DATA, env)
    windows_env_data = update_dict(WINDOWS_ENV_DATA, env)
    mac_m1_env_data = update_dict(MAC_M1_ENV_DATA, env)
    env_file = []
    native.genrule(
        name = name + "_env",
        tools = qt_plugin_data,
        outs = ["qt_env1.ini"],
        cmd = select({
            "@platforms//os:linux":
                "echo $$\"LD_LIBRARY_PATH: $(location @qt_linux_x86_64//:lib)\" > $@ \
                    $$\"\r\nQT_QPA_PLATFORM_PLUGIN_PATH: $(location @qt_linux_x86_64//:plugins)/platforms\" > $@ \
                    $$\"\r\nQML2_IMPORT_PATH: $(location @qt_linux_x86_64//:qml)\" > $@ \
                    $$\"\r\nQT_PLUGIN_PATH: $(location @qt_linux_x86_64//:plugins)\" > $@",
            "@rules_qt//:osx_x86_64":
                "echo $$\"QT_QPA_PLATFORM_PLUGIN_PATH: $(location @qt_mac_x86_64//:plugins)/platforms\" > $@ \
                    $$\"\r\nQML2_IMPORT_PATH: $(location @qt_mac_x86_64//:qml)\" > $@ \
                    $$\"\r\nQT_PLUGIN_PATH: $(location @qt_mac_x86_64//:plugins)\" > $@",
            "@rules_qt//:osx_arm64":
                "echo $$\"QT_QPA_PLATFORM_PLUGIN_PATH: $(location @qt_mac_aarch64//:plugins)/platforms\" > $@ \
                    $$\"\r\nQML2_IMPORT_PATH: $(location @qt_mac_aarch64//:qml)\" > $@ \
                    $$\"\r\nQT_PLUGIN_PATH: $(location @qt_mac_aarch64//:plugins)\" > $@",
            "@platforms//os:windows":
                "echo $$\"QT_QPA_PLATFORM_PLUGIN_PATH: $(location @qt_windows_x86_64//:plugins)/platforms\" > $@ \
                    $$\"\r\nQML2_IMPORT_PATH: $(location @qt_windows_x86_64//:qml)\" > $@ \
                    $$\"\r\nQT_PLUGIN_PATH: $(location @qt_windows_x86_64//:plugins)\" > $@",
        }),
    )
    env_file.append("qt_env1.ini")
    cc_binary(
        name = name,
        srcs = srcs,
        deps = deps,
        copts = copts + select({
            "@platforms//os:windows": [],
            "//conditions:default": ["-fPIC"],
        }),
        data = qt_plugin_data + env_file + data,
        env = select({
            "@platforms//os:linux": linux_env_data,
            "@rules_qt//:osx_x86_64": mac_x64_env_data,
            "@rules_qt//:osx_arm64": mac_m1_env_data,
            "@platforms//os:windows": windows_env_data,
        }),
        **kwargs
    )