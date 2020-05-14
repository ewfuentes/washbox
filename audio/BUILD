
cc_binary(
   name = "main",
   srcs = ["main.cc"],
)

cc_binary(
   name = "list_audio_devices",
   srcs = ["list_audio_devices.cc"],
   deps = ["@portaudio//:portaudio"],
   linkopts = ["-lpthread", "-lasound"],
)

