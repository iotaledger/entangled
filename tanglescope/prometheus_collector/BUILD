cc_library(
    name = "prometheus_collector",
    srcs = glob([
        "*.cpp",
        "*.hpp",
    ]),
    hdrs = glob(["*.hpp"]),
    include_prefix = "iota/tanglescope/prometheus_collector",
    visibility = ["//visibility:public"],
    deps = [
        "//tanglescope/collector",
        "@prometheus_cpp",
    ],
)
