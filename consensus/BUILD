load(":conf.bzl", "CONSENSUS_MAINNET_VARIABLES")
load(":conf.bzl", "CONSENSUS_TESTNET_VARIABLES")

config_setting(
    name = "mainnet",
    values = {"define": "network=mainnet"},
)

config_setting(
    name = "testnet",
    values = {"define": "network=testnet"},
)

cc_library(
    name = "conf",
    srcs = ["conf.c"],
    hdrs = ["conf.h"],
    defines = select({
        ":mainnet": CONSENSUS_MAINNET_VARIABLES,
        ":testnet": CONSENSUS_TESTNET_VARIABLES,
        "//conditions:default": CONSENSUS_MAINNET_VARIABLES,
    }),
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:flex_trit",
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
        "//consensus/milestone_tracker",
        "//consensus/snapshot",
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
        "//common/trinary:trit_array",
    ],
)
