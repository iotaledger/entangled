cc_library(
    name = "statements",
    srcs = ["statements.c"],
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = ["//common/storage"],
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
