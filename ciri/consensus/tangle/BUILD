cc_library(
    name = "tangle",
    srcs = ["tangle.c"],
    hdrs = ["tangle.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/snapshot:state_delta",
        "//ciri/storage",
        "//common:errors",
        "//common/model:bundle",
        "//common/model:transaction",
        "//utils:logger_helper",
        "//utils/containers/hash:hash243_queue",
        "//utils/containers/hash:hash243_set",
        "//utils/containers/hash:hash81_queue",
        "@com_github_uthash//:uthash",
    ],
)

cc_library(
    name = "traversal",
    srcs = ["traversal.c"],
    hdrs = ["traversal.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/tangle",
        "//ciri/storage:pack",
        "//common/model:transaction",
        "//utils/containers/hash:hash243_set",
        "//utils/containers/hash:hash243_stack",
    ],
)
