cc_library(
    name = "storage",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
        "//utils:logger_helper",
    ],
)

cc_library(
    name = "pack",
    srcs = ["pack.c"],
    hdrs = ["pack.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
        "//utils:logger_helper",
    ],
)
