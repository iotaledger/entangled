cc_library(
    name = "sqlite3_storage",
    srcs = [
        "connection.c",
        "storage.c",
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/storage",
        "//common/storage/sql:connection",
        "//utils:logger_helper",
        "@sqlite3",
    ],
)
