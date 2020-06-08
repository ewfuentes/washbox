
package(default_visibility=["//audio:__subpackages__"])

proto_library(
    name = "audio_samples_proto",
    srcs = ["audio_samples.proto"],
)

cc_proto_library(
    name = "audio_samples_proto_cc",
    deps = [":audio_samples_proto"],
)

cc_library(
    name = "audio_samples_to_proto",
    hdrs = ["audio_samples_to_proto.hh"],
    srcs = ["audio_samples_to_proto.cc"],
    deps = [
      ":audio_samples_proto_cc",
      "//audio:audio_samples",
    ],
)