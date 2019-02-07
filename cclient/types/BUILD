cc_library(
    name = "types",
    srcs = [
        "types.c",
    ],
    hdrs = [
        "types.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:transaction",
        "//common/trinary:trit_array",
        "//common/trinary:tryte",
        "//utils:logger_helper",
        "//utils/containers/hash:hash243_queue",
        "//utils/containers/hash:hash243_stack",
        "//utils/containers/hash:hash8019_queue",
        "//utils/containers/hash:hash8019_stack",
        "//utils/containers/hash:hash81_queue",
        "//utils/containers/hash:hash_array",
        "@com_github_uthash//:uthash",
    ],
)
