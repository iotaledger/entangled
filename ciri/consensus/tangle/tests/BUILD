genrule(
    name = "db_file",
    srcs = ["//common/storage/sql:tangle-schema"],
    outs = ["tangle.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
)

cc_test(
    name = "test_tangle",
    timeout = "moderate",
    srcs = ["test_tangle.c"],
    data = [":db_file"],
    deps = [
        "//ciri/consensus/tangle",
        "//ciri/consensus/test_utils",
        "@unity",
    ],
)
