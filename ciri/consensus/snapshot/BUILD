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
        "//ciri/consensus:conf",
        "//ciri/consensus/snapshot:state_delta",
        "//common:errors",
        "//common/model:transaction",
        "//common/trinary:trit_array",
        "//utils:files",
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
        "//ciri/consensus:conf",
        "//common:errors",
        "//common/model:transaction",
        "//common/trinary:flex_trit",
        "//utils:hash_maps",
        "//utils:macros",
        "//utils/containers/hash:hash_int64_t_map",
    ],
)

cc_library(
    name = "snapshots_provider",
    srcs = [
        "snapshots_provider.c",
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
    ],
    hdrs = ["snapshots_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":snapshots_provider",
        "//ciri/consensus:conf",
        "//ciri/consensus/milestone:milestone_service",
        "//ciri/consensus/milestone:milestone_tracker",
        "//ciri/consensus/tangle",
        "//common:errors",
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
        "//ciri/consensus:conf",
        "//common:errors",
        "//common/model:transaction",
        "//utils:files",
        "//utils:macros",
        "//utils/containers/hash:hash_uint64_t_map",
    ],
)
