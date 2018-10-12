cc_library(
    name = "consensus",
    srcs = ["consensus.c"],
    hdrs = ["consensus.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//consensus/bundle_validator",
        "//consensus/cw_rating_calculator",
        "//consensus/entry_point_selector",
        "//consensus/exit_probability_randomizer",
        "//consensus/exit_probability_validator",
        "//consensus/ledger_validator",
        "//consensus/milestone_tracker",
        "//consensus/snapshot",
        "//consensus/tangle",
        "//consensus/tipselection",
        "//consensus/transaction_validator",
    ],
)

cc_library(
    name = "defs",
    hdrs = ["defs.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "model",
    hdrs = ["model.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
    ],
)
