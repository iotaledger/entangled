CONSENSUS_MAINNET_VARIABLES = [
    "SNAPSHOT_CONF_FILE='\"external/snapshot_conf_mainnet/file/snapshot.json\"'",
    "SNAPSHOT_SIG_FILE='\"external/snapshot_sig_mainnet/file/snapshot.sig\"'",
    "SNAPSHOT_FILE='\"external/snapshot_mainnet/file/snapshot.txt\"'",
    "NUM_KEYS_IN_MILESTONE=20",
    "MWM=14",
]

CONSENSUS_TESTNET_VARIABLES = [
    "SNAPSHOT_CONF_FILE='\"external/snapshot_conf_testnet/file/snapshot.json\"'",
    "SNAPSHOT_SIG_FILE='\"\"'",
    "SNAPSHOT_FILE='\"external/snapshot_testnet/file/snapshot.txt\"'",
    "NUM_KEYS_IN_MILESTONE=22",
    "MWM=9",
]

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
