
load("aarch64_toolchain_config.bzl", "aarch64_toolchain_config")

package(default_visibility=["//visibility:public"])

cc_toolchain_suite(
    name="aarch64_suite",
    toolchains = {
      "aarch64": "aarch64_toolchain",
    }
)

filegroup(name="empty")

filegroup(
    name="all",
    srcs = [
        ":aarch64-none-linux-gnu-gcc.sh",
        "@aarch64_none_linux_gnu//:all",
    ]
)

aarch64_toolchain_config(name="aarch64_toolchain_config")

cc_toolchain(
    name="aarch64_toolchain",
    toolchain_identifier="aarch64-none-linux-gnu",
    toolchain_config=":aarch64_toolchain_config",
    all_files=":all",
    compiler_files=":all",
    dwp_files=":empty",
    linker_files=":all",
    objcopy_files=":empty",
    strip_files=":empty",
    supports_param_files=0,
)