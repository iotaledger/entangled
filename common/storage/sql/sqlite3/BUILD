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
