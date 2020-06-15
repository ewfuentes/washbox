
# Fix it so that this no longer needs to be publicly visible
package(default_visibility=["//visibility:public"])

proto_library(
  name = "log_proto",
  srcs = ["log.proto"],
)

cc_proto_library(
  name = "log_proto_cc",
  deps = [":log_proto"],
)