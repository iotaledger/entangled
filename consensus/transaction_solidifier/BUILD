cc_library(
    name = "transaction_solidifier",
    srcs = ["transaction_solidifier.c"],
    hdrs = ["transaction_solidifier.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:transaction",
        "//consensus:conf",
        "//consensus/tangle",
        "//consensus/utils:tangle_traversals",
        "//gossip:tips_cache",
        "//gossip/components:transaction_requester",
        "//utils:logger_helper",
        "//utils/containers/hash:hash243_set",
        "//utils/handles:lock",
        "//utils/handles:thread",
    ],
)
