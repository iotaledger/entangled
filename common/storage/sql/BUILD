cc_library(
    name = "statements",
    srcs = ["statements.c"],
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = ["//common/storage"],
)

filegroup(
    name = "schema",
    srcs = [
        "schema.sql",
    ],
    visibility = ["//visibility:public"],
)
