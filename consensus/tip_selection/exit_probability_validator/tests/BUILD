cc_test(
    name = "test_exit_probability_validator",
    timeout = "moderate",
    srcs = ["exit_probability_validator.c"],
    data = [
        ":db_file",
        ":snapshot.txt",
        "//consensus/snapshot/tests:snapshot_test_files",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//common/storage/tests/helpers",
        "//common/trinary:trit_ptrit",
        "//consensus/test_utils",
        "//consensus/tip_selection/exit_probability_validator",
        "@unity",
    ],
)

genrule(
    name = "db_file",
    srcs = ["//common/storage/sql:tangle-schema"],
    outs = ["ciri.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
)
