
package(default_visibility=["//visibility:public"])

cc_library(
  name = "log_writer",
  hdrs = ["log_writer.hh"],
  srcs = ["log_writer.cc"],
  deps = [
    "//common/logging/proto:log_proto_cc",
  ],
)

cc_test(
  name = "log_writer_test",
  srcs = ["log_writer_test.cc"],
  deps = [
    ":log_writer",
    "@gtest//:gtest_main",
  ],

)