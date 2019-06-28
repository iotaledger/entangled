cc_library(
    name = "ledger_validator_shared",
    hdrs = ["ledger_validator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/snapshot",
        "//common:errors",
        "//utils:hash_maps",
    ],
)

cc_library(
    name = "ledger_validator",
    srcs = ["ledger_validator.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":ledger_validator_shared",
        "//ciri/consensus/bundle_validator",
        "//ciri/consensus/milestone:milestone_tracker_shared",
        "//ciri/consensus/snapshot",
        "//ciri/consensus/snapshot:snapshots_provider",
        "//ciri/consensus/tangle",
        "//ciri/consensus/tangle:traversal",
        "//utils:hash_maps",
        "//utils/containers/hash:hash243_stack",
    ],
)
