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
        "//common/model:transaction",
        "//consensus/snapshot",
        "@unity",
    ],
)

cc_test(
    name = "test_state_diff",
    srcs = ["test_state_diff.c"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/helpers:digest",
        "//consensus/snapshot",
        "//consensus/snapshot:state_diff",
        "@unity",
    ],
)
