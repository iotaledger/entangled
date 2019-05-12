load(":conf.bzl", "CONSENSUS_MAINNET_VARIABLES")
load(":conf.bzl", "CONSENSUS_TESTNET_VARIABLES")

cc_library(
    name = "conf",
    srcs = ["conf.c"],
    hdrs = ["conf.h"],
    defines = select({
        "//ciri:mainnet": CONSENSUS_MAINNET_VARIABLES,
        "//ciri:testnet": CONSENSUS_TESTNET_VARIABLES,
        "//conditions:default": CONSENSUS_MAINNET_VARIABLES,
    }),
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/crypto/sponge",
        "//common/trinary:flex_trit",
        "//consensus/snapshot/local_snapshots:conf",
        "//utils:logger_helper",
        "@cJSON",
    ],
)

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
        "//consensus/milestone:milestone_service",
        "//consensus/milestone:milestone_tracker",
        "//consensus/snapshot",
        "//consensus/snapshot:snapshots_provider",
        "//consensus/snapshot:snapshots_service",
        "//consensus/snapshot/local_snapshots:local_snapshots_manager",
        "//consensus/tangle",
        "//consensus/tip_selector",
        "//consensus/transaction_solidifier",
        "//consensus/transaction_validator",
    ],
)

cc_library(
    name = "model",
    hdrs = ["model.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:flex_trit",
    ],
)
