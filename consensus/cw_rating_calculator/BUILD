cc_library(
    name = "cw_rating_calculator",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
        "//consensus:model",
        "//consensus/tangle",
        "//utils:hash_containers",
        "//utils:logger_helper",
        "//utils/containers:bitset",
        "@com_github_uthash//:uthash",
    ],
)
