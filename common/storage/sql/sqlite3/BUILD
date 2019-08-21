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
        "//common/model:milestone",
        "//common/model:transaction",
        "//common/storage/sql:statements",
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
    srcs = ["//common/storage/sql/sqlite3:tangle-schema"],
    outs = ["tangle.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
    visibility = ["//visibility:public"],
)

genrule(
    name = "spent_addresses_db",
    srcs = ["//common/storage/sql/sqlite3:spent-addresses-schema"],
    outs = ["spent-addresses.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
    visibility = ["//visibility:public"],
)
