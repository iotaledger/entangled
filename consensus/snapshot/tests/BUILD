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
    srcs = ["test_snapshot.c"],
    data = [
        ":snapshot_test_files",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//common/model:transaction",
        "//consensus/snapshot",
        "@unity",
    ],
)
