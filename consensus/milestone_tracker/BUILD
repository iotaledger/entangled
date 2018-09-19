cc_library(
    name = "milestone_tracker",
    srcs = ["milestone_tracker.c"],
    hdrs = ["milestone_tracker.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/conf:conf_values",
        "//common/sign/v1:iss_curl",
        "//consensus/bundle_validator",
        "//consensus/snapshot",
        "//mam/v1:merkle",
        "//utils:macros",
        "//utils:time",
        "//utils/handles:thread",
    ],
)
