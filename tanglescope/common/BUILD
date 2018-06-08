cc_library(
    name = "common",
    srcs = glob([
        "*.cpp",
        "*.hpp",
    ]),
    hdrs = glob(["*.hpp"]),
    defines = [
        "ZMQ_BUILD_DRAFT_API",
    ],
    include_prefix = "iota/tanglescope/common",
    visibility = ["//visibility:public"],
    deps = [
        "//cppclient:beast",
        "@cppzmq",
        "@glog",
        "@libcuckoo",
        "@rxcpp",
        "@yaml_cpp",
    ],
)

cc_test(
    name = "common_test",
    srcs = ["tests/iri.cpp"],
    deps = [
        ":common",
        "@com_github_gflags_gflags//:gflags",
        "@gtest//:gtest_main",
    ],
)
