cc_test(
    name = "test_exit_probability_validator",
    srcs = [
        "exit_probability_validator.c",
    ],
    data = [
        ":db_file",
        ":snapshot.txt",
        "//consensus/snapshot/tests:snapshot_test_files",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//common/storage/tests/helpers",
        "//common/trinary:trit_ptrit",
        "//consensus/exit_probability_validator",
        "//consensus/test_utils",
        "@unity",
    ],
)

genrule(
    name = "db_file",
    srcs = ["//common/storage/sql:schema"],
    outs = ["ciri.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
)
