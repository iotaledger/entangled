filegroup(
    name = "snapshot_files",
    srcs = [
        "@snapshot_conf_mainnet//file",
        "@snapshot_mainnet//file",
        "@snapshot_sig_mainnet//file",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "snapshot",
    srcs = ["snapshot.c"],
    hdrs = ["snapshot.h"],
    data = [":snapshot_files"],
    defines = [
        "SNAPSHOT_CONF_MAINNET='\"external/snapshot_conf_mainnet/file/snapshot.json\"'",
        "SNAPSHOT_SIG_MAINNET='\"external/snapshot_sig_mainnet/file/snapshot.sig\"'",
        "SNAPSHOT_MAINNET='\"external/snapshot_mainnet/file/snapshot.txt\"'",
        "SNAPSHOT_CONF_TESTNET='\"external/snapshot_conf_testnet/file/snapshot.json\"'",
        "SNAPSHOT_SIG_TESTNET='\"external/snapshot_sig_testnet/file/snapshot.sig\"'",
        "SNAPSHOT_TESTNET='\"external/snapshot_testnet/file/snapshot.txt\"'",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:transaction",
        "//common/trinary:trit_array",
        "//consensus:defs",
        "//consensus/snapshot:state_delta",
        "//utils:logger_helper",
        "//utils:signed_files",
        "//utils/handles:rw_lock",
    ],
)

cc_library(
    name = "state_delta",
    srcs = ["state_delta.c"],
    hdrs = ["state_delta.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:flex_trit",
        "//utils:hash_containers",
    ],
)
