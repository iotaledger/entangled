cc_test(
    name = "test_milestone_tracker",
    timeout = "moderate",
    srcs = ["test_milestone_tracker.c"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/milestone:milestone_tracker",
        "//ciri/consensus/snapshot:snapshots_provider",
        "//ciri/consensus/test_utils",
        "@unity",
    ],
)
