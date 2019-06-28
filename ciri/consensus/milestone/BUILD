cc_library(
    name = "milestone_tracker_shared",
    hdrs = ["milestone_tracker.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/crypto/sponge",
        "//utils/containers/hash:hash243_queue",
        "//utils/handles:cond",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "milestone_tracker",
    srcs = ["milestone_tracker.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":milestone_tracker_shared",
        "//ciri/consensus/bundle_validator",
        "//ciri/consensus/ledger_validator",
        "//ciri/consensus/snapshot",
        "//ciri/consensus/snapshot:snapshots_provider",
        "//ciri/consensus/transaction_solidifier",
        "//common/crypto/iss/v1:iss",
        "//utils:macros",
        "//utils:time",
    ],
)

cc_library(
    name = "milestone_service",
    srcs = [
        "milestone_service.c",
    ],
    hdrs = ["milestone_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus:conf",
        "//ciri/consensus/snapshot",
        "//ciri/consensus/tangle",
        "//common:errors",
        "//common/model:milestone",
    ],
)
