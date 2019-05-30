cc_library(
    name = "shared",
    srcs = glob(
        ["**/*.cpp"],
        exclude = [
            "statscollector.cpp",
            "tests/**/*.cpp",
        ],
    ),
    hdrs = glob(
        ["**/*.hpp"],
        exclude = ["tests/**/*.hpp"],
    ),
    deps = [
        "//tanglescope/common",
        "//tanglescope/prometheus_collector",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
    ],
)

cc_test(
    name = "tests",
    timeout = "short",
    srcs = glob([
        "tests/**/*.cpp",
        "**/*.hpp",
    ]),
    deps = [
        ":shared",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "statscollector",
    srcs = ["statscollector.cpp"],
    hdrs = glob(["statscollector.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        ":shared",
        "@prometheus_cpp",
    ],
)
