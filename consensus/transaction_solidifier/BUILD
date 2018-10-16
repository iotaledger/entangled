cc_library(
    name = "transaction_solidifier",
    srcs = ["transaction_solidifier.c"],
    hdrs = ["transaction_solidifier.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:transaction",
        "//common/trinary:trit_array",
        "//consensus:defs",
        "//consensus/tangle",
        "//consensus/utils:tangle_traversals",
        "//gossip/components:requester",
        "//utils:hash_containers",
        "//utils:logger_helper",
        "//utils/handles:lock",
        "//utils/handles:thread",
    ],
)
