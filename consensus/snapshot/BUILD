cc_library(
    name = "snapshot",
    srcs = ["snapshot.c"],
    hdrs = ["snapshot.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:transaction",
        "//common/trinary:trit_array",
        "//consensus:defs",
        "//consensus/snapshot:state_diff",
        "//utils:logger_helper",
        "//utils/handles:rw_lock",
    ],
)

cc_library(
    name = "state_diff",
    srcs = ["state_diff.c"],
    hdrs = ["state_diff.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:flex_trit",
        "@com_github_uthash//:uthash",
    ],
)
