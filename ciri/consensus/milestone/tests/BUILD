genrule(
    name = "db_file",
    srcs = ["//common/storage/sql:tangle-schema"],
    outs = ["ciri.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
)

cc_test(
    name = "test_milestone_tracker",
    timeout = "moderate",
    srcs = ["test_milestone_tracker.c"],
    data = [":db_file"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/milestone:milestone_tracker",
        "//ciri/consensus/snapshot:snapshots_provider",
        "//ciri/consensus/test_utils",
        "@unity",
    ],
)
