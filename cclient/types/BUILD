cc_library(
    name = "types",
    srcs = [
        "types.c",
        "types.h",
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
        "@com_github_uthash//:uthash",
    ],
)
