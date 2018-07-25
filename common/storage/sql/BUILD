cc_library(
    name = "connection",
    hdrs = glob(["*.h"]),
    srcs = ["statements.c",],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:transaction",
        "//common/storage:storage",
        "//common:logger_helper",
    ],
)
