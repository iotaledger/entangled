cc_library(
    name = "storage_sqlite3",
    srcs = [
        "connection.c",
        "storage.c",
        "wrappers.c",
    ],
    hdrs = [
        "connection.h",
        "wrappers.h",
    ],
    data = [
        ":spent_addresses_db",
        ":tangle_db",
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/storage/sql:statements",
        "//common/model:milestone",
        "//common/model:transaction",
        "//utils:logger_helper",
        "//utils:time",
        "@sqlite3",
    ],
)

filegroup(
    name = "tangle-schema",
    srcs = ["tangle-schema.sql"],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "spent-addresses-schema",
    srcs = ["spent-addresses-schema.sql"],
    visibility = ["//visibility:public"],
)

genrule(
    name = "tangle_db",
    srcs = ["//ciri/storage/sql/sqlite3:tangle-schema"],
    outs = ["tangle.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
    visibility = ["//visibility:public"],
)

genrule(
    name = "spent_addresses_db",
    srcs = ["//ciri/storage/sql/sqlite3:spent-addresses-schema"],
    outs = ["spent-addresses.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "test_utils_sqlite3",
    srcs = ["test_utils.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":storage_sqlite3",
        "//ciri/storage:test_utils_hdr",
        "//ciri/utils:files",
    ],
)
