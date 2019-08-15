cc_library(
    name = "tangle",
    srcs = ["tangle.c"],
    hdrs = ["tangle.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/snapshot:state_delta",
        "//common:errors",
        "//common/model:bundle",
        "//common/model:transaction",
        "//common/storage/sql/sqlite3:storage_sqlite3",
        "//common/trinary:trit_array",
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
        "//common/model:transaction",
        "//common/storage:pack",
        "//utils/containers/hash:hash243_set",
        "//utils/containers/hash:hash243_stack",
    ],
)
