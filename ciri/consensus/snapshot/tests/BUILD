filegroup(
    name = "snapshot_test_files",
    srcs = [
        "snapshot.txt",
        "snapshot_badly_formatted.txt",
        "snapshot_conf.json",
        "snapshot_inconsistent.txt",
        "snapshot_invalid_supply.txt",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "test_snapshot",
    timeout = "short",
    srcs = ["test_snapshot.c"],
    data = [
        ":snapshot_test_files",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/snapshot",
        "//common/model:transaction",
        "@unity",
    ],
)

cc_test(
    name = "test_state_delta",
    timeout = "short",
    srcs = ["test_state_delta.c"],
    data = [
        ":snapshot_test_files",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/snapshot",
        "//common/model:transaction",
        "@unity",
    ],
)

cc_test(
    name = "test_snapshot_metadata",
    timeout = "short",
    srcs = ["test_snapshot_metadata.c"],
    data = [
        ":snapshot_test_files",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/snapshot:snapshot_metadata",
        "//common/model:transaction",
        "//utils:time",
        "@unity",
    ],
)
