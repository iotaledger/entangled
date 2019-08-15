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
