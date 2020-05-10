# WORKSPACE

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
  name = "portaudio",
  urls = ["http://www.portaudio.com/archives/pa_stable_v190600_20161030.tgz"],
  build_file = "@//third_party:BUILD.portaudio",
  strip_prefix="portaudio",
  sha256="f5a21d7dcd6ee84397446fa1fa1a0675bb2e8a4a6dceb4305a8404698d8d1513"
)

