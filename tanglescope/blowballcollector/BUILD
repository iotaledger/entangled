cc_library(
    name = "blowballcollector",
    srcs = ["blowballcollector.cpp"],
    hdrs = glob(["*.hpp"]),
    include_prefix = "iota/tanglescope",
    visibility = ["//visibility:public"],
    deps = [
        "//tanglescope/common",
        "//tanglescope/prometheus_collector",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
        "@prometheus_cpp",
    ],
)
