cc_library(
    name = "ledger_validator_shared",
    hdrs = ["ledger_validator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//consensus/snapshot",
        "//utils:hash_maps",
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
        "//consensus/utils:tangle_traversals",
        "//utils:hash_maps",
        "//utils/containers/hash:hash243_stack",
    ],
)
