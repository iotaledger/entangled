cc_library(
    name = "test_storage_common",
    srcs = ["test_storage.c"],
    visibility = ["//visibility:private"],
    deps = [
        "//common/helpers:digest",
        "//common/model:milestone",
        "//common/storage",
        "//common/storage/tests/helpers",
        "//common/trinary:trit_ptrit",
        "//utils:files",
        "//utils/containers/hash:hash243_set",
        "@unity",
    ],
)

genrule(
    name = "db_file_sqlite3",
    srcs = ["//common/storage/sql:tangle-schema"],
    outs = ["ciri.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
)

cc_test(
    name = "test_storage_sqlite3",
    timeout = "moderate",
    data = [":db_file_sqlite3"],
    visibility = ["//visibility:public"],
    deps = [
        ":test_storage_common",
        "//common/storage/sql/sqlite3:storage_sqlite3",
    ],
)

cc_test(
    name = "test_storage_mariadb",
    timeout = "moderate",
    # data = [":db_file_sqlite3"],
    visibility = ["//visibility:public"],
    deps = [
        ":test_storage_common",
        "//common/storage/sql/mariadb:storage_mariadb",
    ],
)
