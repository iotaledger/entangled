cc_library(
    name = "ledger_validator_shared",
    hdrs = ["ledger_validator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
    ],
)

cc_library(
    name = "ledger_validator",
    srcs = ["ledger_validator.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":ledger_validator_shared",
        "//consensus/milestone_tracker:milestone_tracker_shared",
        "//consensus/snapshot",
        "//consensus/tangle",
    ],
)
