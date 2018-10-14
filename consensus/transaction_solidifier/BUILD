cc_library(
    name = "transaction_solidifier",
    srcs = ["transaction_solidifier.c"],
    hdrs = ["transaction_solidifier.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
        "//consensus:defs",
        "//consensus/tangle",
        "//gossip/components:requester",
        "//utils:logger_helper",
        "//utils/handles:thread",
    ],
)
