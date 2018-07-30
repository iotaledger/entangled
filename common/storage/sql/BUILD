cc_library(
    name = "connection",
    srcs = ["statements.c"],
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common:logger_helper",
        "//common/model:transaction",
        "//common/storage",
    ],
)

filegroup(
    name = "schema",
    srcs = [
        "schema.sql",
    ],
    visibility = ["//visibility:public"],
)
