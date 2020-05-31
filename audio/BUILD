load("@io_bazel_rules_docker//container:image.bzl", "container_image")

cc_binary(
   name = "main",
   srcs = ["main.cc"],
)

container_image(
    name = "main_image",
    base = select({"//:aarch64": "@aarch64_base//image", "//conditions:default": "@amd64_base//image"}),
    files = [":main"],
)

cc_binary(
   name = "list_audio_devices",
   srcs = ["list_audio_devices.cc"],
   deps = ["@portaudio//:portaudio"],
)

container_image(
    name = "list_audio_devices_image",
    base = select({"//:aarch64": "@aarch64_base//image", "//conditions:default": "@amd64_base//image"}),
    files = [":list_audio_devices"],
    legacy_run_behavior=False,
    cmd = ["./list_audio_devices"],
    docker_run_flags = "--device /dev/snd",
)
