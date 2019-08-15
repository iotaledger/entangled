cc_test(
    name = "test_storage",
    timeout = "moderate",
    srcs = [
        "test_storage.c",
    ],
    data = [":db_file"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/helpers:digest",
        "//common/storage/sql/sqlite3:sqlite3_storage",
        "//common/storage/tests/helpers",
        "//common/trinary:trit_ptrit",
        "//utils:files",
        "//utils/containers/hash:hash243_set",
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
