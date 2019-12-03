cc_library(
    name = "storage_mariadb",
    srcs = [
        "connection.c",
        "storage.c",
        "wrappers.c",
    ],
    hdrs = [
        "connection.h",
        "wrappers.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/storage/sql:statements",
        "//common/model:milestone",
        "//common/model:transaction",
        "//utils:logger_helper",
        "//utils:time",
        "@mariadb",
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

cc_library(
    name = "test_utils_mariadb",
    srcs = ["test_utils.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":storage_mariadb",
        "//ciri/storage:test_utils_hdr",
    ],
)
