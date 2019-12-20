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
        "//ciri/consensus/snapshot/local_snapshots:conf",
        "//ciri/utils:files",
        "//common:errors",
        "//common/crypto/sponge",
        "//common/trinary:flex_trit",
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
        "//ciri/consensus/bundle_validator",
        "//ciri/consensus/ledger_validator",
        "//ciri/consensus/milestone:milestone_service",
        "//ciri/consensus/milestone:milestone_tracker",
        "//ciri/consensus/snapshot",
        "//ciri/consensus/snapshot:snapshots_provider",
        "//ciri/consensus/snapshot:snapshots_service",
        "//ciri/consensus/snapshot/local_snapshots:local_snapshots_manager",
        "//ciri/consensus/spent_addresses:spent_addresses_service",
        "//ciri/consensus/tangle",
        "//ciri/consensus/tip_selection:tip_selector",
        "//ciri/consensus/tip_selection/cw_rating_calculator",
        "//ciri/consensus/tip_selection/entry_point_selector",
        "//ciri/consensus/tip_selection/exit_probability_randomizer",
        "//ciri/consensus/tip_selection/exit_probability_validator",
        "//ciri/consensus/transaction_solidifier",
        "//ciri/consensus/transaction_validator",
        "//common:errors",
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
