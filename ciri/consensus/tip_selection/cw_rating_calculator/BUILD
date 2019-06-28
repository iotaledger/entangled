cc_library(
    name = "cw_rating_calculator",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus:model",
        "//ciri/consensus/tangle",
        "//common:errors",
        "//common/trinary:trit_array",
        "//utils:hash_maps",
        "//utils:logger_helper",
        "//utils/containers:bitset",
        "//utils/containers/hash:hash243_stack",
        "//utils/containers/hash:hash_int64_t_map",
        "@com_github_uthash//:uthash",
    ],
)
