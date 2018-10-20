cc_library(
    name = "milestone_tracker_shared",
    hdrs = ["milestone_tracker.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "milestone_tracker",
    srcs = ["milestone_tracker.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":milestone_tracker_shared",
        "//ciri/conf:conf_values",
        "//common/sign/v1:iss_curl",
        "//consensus/bundle_validator",
        "//consensus/ledger_validator",
        "//consensus/snapshot",
        "//utils:macros",
        "//utils:merkle",
        "//utils:time",
    ],
)
