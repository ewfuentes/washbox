load("@io_bazel_rules_docker//container:image.bzl", "container_image")

cc_library(
  name = "audio_samples",
  hdrs = ["audio_samples.hh"],
  visibility = ["//audio:__subpackages__"],
)

cc_binary(
  name = "microphone_publisher",
  srcs = ["microphone_publisher.cc"],
  deps = [
    "//audio/proto:audio_samples_to_proto",
    "@gflags//:gflags",
    "@portaudio//:portaudio",
     "@cppzmq//:cppzmq",
  ]
)

cc_binary(
  name = "audio_recorder",
  srcs = ["audio_recorder.cc"],
  deps = [
    "@gflags//:gflags",
    "@cppzmq//:cppzmq",
    "//audio/proto:audio_samples_to_proto",
    "//common/logging:log_writer",
  ],
)

container_image(
    name = "microphone_publisher_image",
    base = select({"//:aarch64": "@aarch64_base//image", "//conditions:default": "@amd64_base//image"}),
    files = [":microphone_publisher"],
    legacy_run_behavior=False,
    cmd = ["./microphone_publisher"],
    docker_run_flags = "--device /dev/snd",
)

cc_binary(
   name = "main",
   srcs = ["main.cc"],
   deps = [
     "@cppzmq//:cppzmq",
   ]
)

cc_binary(
   name = "client",
   srcs = ["client.cc"],
   deps = [
     "@cppzmq//:cppzmq",
   ]
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
