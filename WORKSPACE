# WORKSPACE

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

new_local_repository(
    name = "libasound2",
    path = "/usr",
    build_file = "@//third_party:BUILD.libasound2",
)

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
    tag="aarch64"
)

container_pull(
    name="amd64_base",
    registry="index.docker.io",
    repository="ewfuentes/washbox",
    tag="amd64"
)