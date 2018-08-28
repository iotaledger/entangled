cc_library(
    name = "connection",
    srcs = ["statements.c"],
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:milestone",
        "//common/model:transaction",
        "//common/storage",
        "//utils:logger_helper",
    ],
)

filegroup(
    name = "schema",
    srcs = [
        "schema.sql",
    ],
    visibility = ["//visibility:public"],
)
