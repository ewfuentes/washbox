# WORKSPACE

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
  name = "portaudio",
  urls = ["http://www.portaudio.com/archives/pa_stable_v190600_20161030.tgz"],
  build_file = "@//third_party:BUILD.portaudio",
  strip_prefix="portaudio",
  sha256="f5a21d7dcd6ee84397446fa1fa1a0675bb2e8a4a6dceb4305a8404698d8d1513"
)

http_archive(
  name = "aarch64_none_linux_gnu",
  urls = ["https://developer.arm.com/-/media/Files/downloads/gnu-a/9.2-2019.12/binrel/gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu.tar.xz"],
  sha256="8dfe681531f0bd04fb9c53cf3c0a3368c616aa85d48938eebe2b516376e06a66",
  strip_prefix="gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu",
  build_file = "@//third_party:BUILD.aarch64_none_linux_gnu"
)
