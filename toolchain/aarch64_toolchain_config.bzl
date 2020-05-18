load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")
load("@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
    "tool_path", "feature", "flag_group", "flag_set", "with_feature_set")

def _impl(ctx):
    paths = [
      ("gcc", "aarch64-none-linux-gnu-gcc.sh"),
      ("ld", "aarch64-none-linux-gnu-gcc.sh"),
      ("ar", "aarch64-none-linux-gnu-ar.sh"),
      ("cpp", "aarch64-none-linux-gnu-cpp.sh"),
      ("gcov", "aarch64-none-linux-gnu-gcov.sh"),
      ("nm", "aarch64-none-linux-gnu-nm.sh"),
      ("objdump", "aarch64-none-linux-gnu-objdump.sh"),
      ("strip", "aarch64-none-linux-gnu-strip.sh"),
    ]
    tool_paths = [tool_path(name=name, path=path) for name, path in paths]

    toolchain_include_directories_feature = feature(
        name = "toolchain_include_directories",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.assemble,
                    ACTION_NAMES.preprocess_assemble,
                    ACTION_NAMES.linkstamp_compile,
                    ACTION_NAMES.cpp_compile,
                    ACTION_NAMES.cpp_header_parsing,
                    ACTION_NAMES.cpp_module_compile,
                    ACTION_NAMES.cpp_module_codegen,
                    ACTION_NAMES.lto_backend,
                    ACTION_NAMES.clif_match,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-std=c++17",
                            "-isystem",
                            "external/aarch64_none_linux_gnu/aarch64-none-linux-gnu/include/c++/9.2.1",
                            "-isystem",
                            "external/aarch64_none_linux_gnu/aarch64-none-linux-gnu/include/c++/9.2.1/aarch64-none-linux-gnu",
                            "-isystem",
                            "external/aarch64_none_linux_gnu/aarch64-none-linux-gnu/include/c++/9.2.1/backward",
                            "-isystem",
                            "external/aarch64_none_linux_gnu/lib/gcc/aarch64-none-linux-gnu/9.2.1/include",
                            "-isystem",
                            "external/aarch64_none_linux_gnu/lib/gcc/aarch64-none-linux-gnu/9.2.1/include-fixed",
                            "-isystem",
                            "external/aarch64_none_linux_gnu/aarch64-none-linux-gnu/include",
                            "-isystem",
                            "external/aarch64_none_linux_gnu/aarch64-none-linux-gnu/libc/usr/include",
                        ]
                    )
                ]
            ),
            flag_set(
                actions = [
                    ACTION_NAMES.c_compile,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-isystem",
                            "external/aarch64_none_linux_gnu/aarch64-none-linux-gnu/include/c++/9.2.1",
                            "-isystem",
                            "external/aarch64_none_linux_gnu/aarch64-none-linux-gnu/include/c++/9.2.1/aarch64-none-linux-gnu",
                            "-isystem",
                            "external/aarch64_none_linux_gnu/aarch64-none-linux-gnu/include/c++/9.2.1/backward",
                            "-isystem",
                            "external/aarch64_none_linux_gnu/lib/gcc/aarch64-none-linux-gnu/9.2.1/include",
                            "-isystem",
                            "external/aarch64_none_linux_gnu/lib/gcc/aarch64-none-linux-gnu/9.2.1/include-fixed",
                            "-isystem",
                            "external/aarch64_none_linux_gnu/aarch64-none-linux-gnu/include",
                            "-isystem",
                            "external/aarch64_none_linux_gnu/aarch64-none-linux-gnu/libc/usr/include",
                        ]
                    )
                ]
            )

        ]
    )

    default_link_flags_feature = feature(
        name = "default_link_flags",
        enabled=True,
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.cpp_link_executable,
                    ACTION_NAMES.cpp_link_dynamic_library,
                    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-lstdc++",
                            "-Wl,-z,relro,-z,now",
                            "-pass-exit-codes",
                            "-Wl,--build-id=md5",
                            "-Wl,--hash-style=gnu",
                        ]
                    )
                ]
            ),
        ]
    )

    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        toolchain_identifier = "aarch64-none-linux-gnu",
        host_system_name = "i686-unknown-linux-gnu",
        target_system_name = "aarch64-none-linux-gnu",
        target_cpu = "aarch64",
        target_libc = "aarch64",
        compiler = "aarch64-none-linux-gnu",
        abi_version = "aarch64",
        abi_libc_version = "aarch64",
        tool_paths = tool_paths,
        features = [toolchain_include_directories_feature, default_link_flags_feature],
        cxx_builtin_include_directories = [
            "external/aarch64_none_linux_gnu/aarch64-none-linux-gnu/include/c++/9.2.1",
            "external/aarch64_none_linux_gnu/aarch64-none-linux-gnu/include/c++/9.2.1/aarch64-none-linux-gnu",
            "external/aarch64_none_linux_gnu/aarch64-none-linux-gnu/include/c++/9.2.1/backward",
            "external/aarch64_none_linux_gnu/lib/gcc/aarch64-none-linux-gnu/9.2.1/include",
            "external/aarch64_none_linux_gnu/lib/gcc/aarch64-none-linux-gnu/9.2.1/include-fixed",
            "external/aarch64_none_linux_gnu/aarch64-none-linux-gnu/include",
            "external/aarch64_none_linux_gnu/aarch64-none-linux-gnu/libc/usr/include",
        ]
    )

aarch64_toolchain_config = rule (
    implementation = _impl,
    attrs = {},
    provides = [CcToolchainConfigInfo]
)