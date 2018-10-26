cc_library(
    name = "tangle",
    srcs = ["tangle.c"],
    hdrs = ["tangle.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:transaction",
        "//common/storage/sql/sqlite3:sqlite3_storage",
        "//common/trinary:trit_array",
        "//consensus/snapshot:state_delta",
        "//utils:logger_helper",
        "//utils/containers/hash:hash243_set",
        "@com_github_uthash//:uthash",
    ],
)
