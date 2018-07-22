cc_library(
    name = "sqlite3_storage",
    srcs = [
        "connection.cc",
        "storage.cc",
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/storage",
        "//common/storage/sql:connection",
        "@sqlite3",
    ],
)
