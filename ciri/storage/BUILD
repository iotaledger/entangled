config_setting(
    name = "sqlite3",
    values = {"define": "storage=sqlite3"},
)

config_setting(
    name = "mariadb",
    values = {"define": "storage=mariadb"},
)

cc_library(
    name = "storage_common",
    hdrs = [
        "connection.h",
        "defs.h",
        "storage.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":pack",
        "//ciri/consensus/snapshot:state_delta",
        "//common:errors",
        "//common/model:bundle",
        "//utils:hash_maps",
        "//utils:logger_helper",
        "//utils/containers/hash:hash243_queue",
        "//utils/containers/hash:hash81_queue",
    ],
)

cc_library(
    name = "storage",
    visibility = ["//visibility:public"],
    deps = select({
        ":sqlite3": ["//ciri/storage/sql/sqlite3:storage_sqlite3"],
        ":mariadb": ["//ciri/storage/sql/mariadb:storage_mariadb"],
        "//conditions:default": ["//ciri/storage/sql/sqlite3:storage_sqlite3"],
    }),
)

cc_library(
    name = "pack",
    srcs = ["pack.c"],
    hdrs = ["pack.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:flex_trit",
    ],
)

cc_library(
    name = "test_utils_hdr",
    hdrs = ["test_utils.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":storage_common",
        "//common:errors",
    ],
)

cc_library(
    name = "test_utils",
    visibility = ["//visibility:public"],
    deps = select({
        ":sqlite3": ["//ciri/storage/sql/sqlite3:test_utils_sqlite3"],
        ":mariadb": ["//ciri/storage/sql/mariadb:test_utils_mariadb"],
        "//conditions:default": ["//ciri/storage/sql/sqlite3:test_utils_sqlite3"],
    }),
)
