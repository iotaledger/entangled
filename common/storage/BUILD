cc_library(
    name = "storage",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
        "//consensus/snapshot:state_delta",
        "//utils:hash_maps",
        "//utils:logger_helper",
        "//utils/containers/hash:hash243_queue",
        "//utils/containers/hash:hash81_queue",
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
    ],
)
