cc_test(
    name = "test_snapshot",
    srcs = ["test_snapshot.c"],
    data = [
        "snapshot.txt",
        "snapshot_badly_formatted.txt",
        "snapshot_inconsistent.txt",
        "snapshot_invalid_supply.txt",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//consensus/snapshot",
        "@unity",
    ],
)
