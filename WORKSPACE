# WORKSPACE

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

new_local_repository(
    name = "libasound2",
    path = "/usr",
    build_file = "@//third_party:BUILD.libasound2",
)

# rules_proto defines abstract rules for building Protocol Buffers.
http_archive(
    name = "rules_proto",
    sha256 = "2490dca4f249b8a9a3ab07bd1ba6eca085aaf8e45a734af92aad0c42d9dc7aaf",
    strip_prefix = "rules_proto-218ffa7dfa5408492dc86c01ee637614f8695c45",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_proto/archive/218ffa7dfa5408492dc86c01ee637614f8695c45.tar.gz",
        "https://github.com/bazelbuild/rules_proto/archive/218ffa7dfa5408492dc86c01ee637614f8695c45.tar.gz",
    ],
)

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")
rules_proto_dependencies()
rules_proto_toolchains()

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

http_archive(
  name = "zmq",
  urls = ["https://github.com/zeromq/libzmq/archive/v4.3.2.tar.gz"],
  strip_prefix="libzmq-4.3.2",
  build_file = "@//third_party:BUILD.zmq",
  sha256 = "02ecc88466ae38cf2c8d79f09cfd2675ba299a439680b64ade733e26a349edeb",
  patches = ["@//third_party/zmq:platform_header.patch"],
)

http_archive(
  name = "cppzmq",
  urls = ["https://github.com/zeromq/cppzmq/archive/v4.6.0.tar.gz"],
  strip_prefix="cppzmq-4.6.0",
  sha256 = "e9203391a0b913576153a2ad22a2dc1479b1ec325beb6c46a3237c669aef5a52",
  build_file="@//third_party:BUILD.cppzmq",
)

http_archive(
  name = "gflags",
  urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
  sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
  strip_prefix="gflags-2.2.2",
)


# Download the rules_docker repository at release v0.14.1
http_archive(
    name = "io_bazel_rules_docker",
    sha256 = "dc97fccceacd4c6be14e800b2a00693d5e8d07f69ee187babfd04a80a9f8e250",
    strip_prefix = "rules_docker-0.14.1",
    urls = ["https://github.com/bazelbuild/rules_docker/releases/download/v0.14.1/rules_docker-v0.14.1.tar.gz"],
)

load(
    "@io_bazel_rules_docker//repositories:repositories.bzl",
    container_repositories = "repositories")

container_repositories()

load(
    "@io_bazel_rules_docker//cc:image.bzl",
    _cc_image_repos = "repositories",
)

_cc_image_repos()

load("@io_bazel_rules_docker//container:container.bzl", "container_pull")

container_pull(
    name="aarch64_base",
    registry="index.docker.io",
    repository="ewfuentes/washbox",
    digest = "sha256:b6385536cff623761214061e54af735b25e2f4e4b1876cd24594a1e54424fdfa"
)

container_pull(
    name="amd64_base",
    registry="index.docker.io",
    repository="ewfuentes/washbox",
    digest = "sha256:08e0be99c86ac7e7b30e91222061ee61d1ebe72e95b71e1b070f6a1f3cd48a2d"
)