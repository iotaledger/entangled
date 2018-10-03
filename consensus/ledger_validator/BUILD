cc_library(
    name = "ledger_validator_shared",
    hdrs = ["ledger_validator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//consensus/snapshot",
        "//utils:traversal",
    ],
)

cc_library(
    name = "ledger_validator",
    srcs = ["ledger_validator.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":ledger_validator_shared",
        "//consensus/bundle_validator",
        "//consensus/milestone_tracker:milestone_tracker_shared",
        "//consensus/snapshot",
        "//consensus/tangle",
        "//gossip/components:requester",
        "//utils:traversal",
    ],
)
