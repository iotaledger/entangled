cc_library(
    name = "echocatcher",
    srcs = ["echocatcher.cpp"],
    hdrs = glob(["*.hpp"]),
    include_prefix = "iota/tanglescope/echocatcher",
    visibility = ["//visibility:public"],
    deps = [
        "//common/helpers",
        "//common/trinary:tryte_long",
        "//cppclient:beast",
        "//tanglescope/common",
        "//tanglescope/prometheus_collector",
        "@com_github_gflags_gflags//:gflags",
        "@glog",
        "@prometheus_cpp",
    ],
)
