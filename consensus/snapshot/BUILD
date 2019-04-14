filegroup(
    name = "snapshot_files",
    srcs = [
        "@snapshot_conf_mainnet//file",
        "@snapshot_conf_testnet//file",
        "@snapshot_mainnet//file",
        "@snapshot_sig_mainnet//file",
        "@snapshot_testnet//file",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "snapshot",
    srcs = ["snapshot.c"],
    hdrs = ["snapshot.h"],
    data = [":snapshot_files"],
    visibility = ["//visibility:public"],
    deps = [
        ":snapshot_metadata",
        "//common:errors",
        "//common/model:transaction",
        "//common/trinary:trit_array",
        "//consensus:conf",
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
        "//utils:hash_maps",
        "//utils/containers/hash:hash_int64_t_map",
    ],
)

cc_library(
    name = "snapshots_provider",
    srcs = [
        "snapshots_provider.c",
        "snapshots_provider.h",
    ],
    hdrs = ["snapshots_provider.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":snapshot",
    ],
)

cc_library(
    name = "snapshots_service",
    srcs = [
        "snapshots_service.c",
        "snapshots_service.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":snapshots_provider",
        "//common:errors",
        "//consensus:conf",
        "//consensus/milestone_tracker",
        "//consensus/tangle",
    ],
)

cc_library(
    name = "snapshot_metadata",
    srcs = [
        "snapshot_metadata.c",
    ],
    hdrs = ["snapshot_metadata.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//consensus:conf",
        "//utils/containers/hash:hash_uint64_t_map",
    ],
)
