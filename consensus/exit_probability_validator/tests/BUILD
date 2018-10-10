cc_test(
    name = "test_exit_probability_validator",
    srcs = [
        "exit_probability_validator.c",
    ],
    data = [
        "snapshot.txt",
        ":db_file",
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
