load("@io_bazel_rules_docker//cc:image.bzl", "cc_image")

config_setting(
    name = "aarch64",
    values = {
      "cpu": "aarch64"
    }
)

cc_binary(
   name = "main",
   srcs = ["main.cc"],
)

cc_image(
    name = "main_image",
    base = select({":aarch64": "@aarch64_base//image", "//conditions:default": "@cc_image_base//image"}),
    binary = ":main",
)

cc_binary(
   name = "list_audio_devices",
   srcs = ["list_audio_devices.cc"],
   deps = ["@portaudio//:portaudio"],
   linkopts = ["-lpthread", "-lasound"],
)

