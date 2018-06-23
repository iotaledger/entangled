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
        "@com_github_google_glog//:glog",
        "@cppzmq",
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
        "@com_google_googletest//:gtest_main",
    ],
)
