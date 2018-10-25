cc_library(
    name = "types",
    srcs = [
        "types.c",
    ],
    hdrs = [
        "types.h",
    ],
    include_prefix = "types",
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:transaction",
        "//common/trinary:trit_array",
        "//common/trinary:tryte",
        "//utils:logger_helper",
        "//utils/containers/hash:hash243_queue",
        "@com_github_uthash//:uthash",
    ],
)
