cc_library(
    name = "defs",
    hdrs = ["defs.h"],
    visibility = ["//visibility:public"],
    deps = ["//common/model:transaction"],
)

cc_library(
    name = "test_storage_common",
    visibility = ["//visibility:private"],
    deps = [
        ":defs",
        "//ciri/storage:storage_common",
        "//ciri/storage:test_utils_hdr",
        "//common/helpers:digest",
        "//common/model:milestone",
        "//common/trinary:add",
        "//common/trinary:trit_ptrit",
        "//utils/containers/hash:hash243_set",
    ],
)

cc_test(
    name = "test_storage_sqlite3",
    timeout = "moderate",
    srcs = ["test_storage.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":test_storage_common",
        "//ciri/storage/sql/sqlite3:storage_sqlite3",
        "//ciri/storage/sql/sqlite3:test_utils_sqlite3",
        "@unity",
    ],
)

cc_test(
    name = "test_storage_mariadb",
    timeout = "moderate",
    srcs = ["test_storage.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":test_storage_common",
        "//ciri/storage/sql/mariadb:storage_mariadb",
        "//ciri/storage/sql/mariadb:test_utils_mariadb",
        "@unity",
    ],
)
