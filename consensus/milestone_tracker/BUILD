cc_library(
    name = "milestone_tracker_shared",
    hdrs = ["milestone_tracker.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//utils/containers/hash:hash243_queue",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "milestone_tracker",
    srcs = ["milestone_tracker.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":milestone_tracker_shared",
        "//common/crypto/iss/v1:iss_curl",
        "//consensus/bundle_validator",
        "//consensus/ledger_validator",
        "//consensus/snapshot",
        "//consensus/transaction_solidifier",
        "//utils:macros",
        "//utils:merkle",
        "//utils:time",
    ],
)
