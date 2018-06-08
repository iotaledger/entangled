cc_library(
    name = "beast",
    srcs = ["beast.cc"],
    hdrs = ["beast.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":api_json",
        "//common/model:transaction",
        "@boost//:beast",
        "@nlohmann//:json",
    ],
)

cc_library(
    name = "api_json",
    srcs = ["api_json.cc"],
    hdrs = ["api_json.h"],
    deps = [
        ":api",
        "//common/helpers:digest",
        "//common/model:transaction",
        "@boost//:config",
        "@boost//:core",
        "@boost//:fusion",
        "@boost//:move",
        "@boost//:range",
        "@boost//:thread",
        "@com_github_gflags_gflags//:gflags",
        "@glog",
        "@nlohmann//:json",
    ],
)

cc_library(
    name = "api",
    hdrs = [
        "api.h",
        "messages.h",
    ],
    visibility = ["//visibility:public"],
    deps = ["@optional_lite"],
)

cc_test(
    name = "tests",
    srcs = glob(["tests/**/*.cc"]),
    deps = [
        ":beast",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)
