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

cc_binary(
   name = "list_audio_devices",
   srcs = ["list_audio_devices.cc"],
   deps = ["@portaudio//:portaudio"],
)

cc_library(
   name = "wav_file",
   hdrs = ["wav_file.hh"],
   srcs = ["wav_file.cc"],
   deps = [
     ":audio_samples",
   ]
)

cc_test(
   name = "wav_file_test",
   srcs = ["wav_file_test.cc"],
   deps = [
     ":wav_file",
     "@gtest//:gtest_main",
   ]
)

cc_binary(
  name = "wav_from_log",
  srcs = ["wav_from_log.cc"],
  deps = [
    ":wav_file",
    "//audio/proto:audio_samples_to_proto",
    "//common/logging/proto:log_proto_cc",
    "@gflags//:gflags",
  ]
)

container_image(
    name = "microphone_publisher_image",
    base = select({"//:aarch64": "@aarch64_base//image", "//conditions:default": "@amd64_base//image"}),
    files = [":microphone_publisher"],
    legacy_run_behavior=False,
    cmd = ["./microphone_publisher"],
    docker_run_flags = "--device /dev/snd",
)

container_image(
    name = "list_audio_devices_image",
    base = select({"//:aarch64": "@aarch64_base//image", "//conditions:default": "@amd64_base//image"}),
    files = [":list_audio_devices"],
    legacy_run_behavior=False,
    cmd = ["./list_audio_devices"],
    docker_run_flags = "--device /dev/snd",
)
