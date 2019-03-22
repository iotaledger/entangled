genrule(
    name = "db_file",
    srcs = ["//common/storage/sql:schema"],
    outs = ["ciri.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
)

cc_test(
    name = "test_milestone_tracker",
    srcs = ["test_milestone_tracker.c"],
    data = [":db_file"],
    visibility = ["//visibility:public"],
    deps = [
        "//consensus/milestone_tracker",
        "//consensus/test_utils",
        "@unity",
    ],
)
