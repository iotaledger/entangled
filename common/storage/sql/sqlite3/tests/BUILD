cc_test(
    name = "test_sqlite3",
    srcs = [
        "test_sqlite3.c",
    ],
    data = [":db_file"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/storage/sql/sqlite3:sqlite3_storage",
        "//common/storage/tests/helpers",
        "//common/trinary:trit_ptrit",
        "//utils:files",
        "@unity",
    ],
)

genrule(
    name = "db_file",
    srcs = ["//common/storage/sql:schema"],
    outs = ["ciri.db"],
    tools = ["@sqlite3//:shell"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
)
