cc_library(
    name = "prometheus_collector",
    srcs = glob([
        "*.cpp",
        "*.hpp",
    ]),
    hdrs = glob(["*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//tanglescope/collector",
        "@prometheus_cpp",
    ],
)
