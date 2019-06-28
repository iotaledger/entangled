cc_library(
    name = "transaction_validator",
    srcs = ["transaction_validator.c"],
    hdrs = ["transaction_validator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus:conf",
        "//ciri/consensus/snapshot:snapshots_provider",
        "//ciri/node/pipeline:transaction_requester_shared",
        "//common:errors",
        "//common/model:transaction",
        "//utils:logger_helper",
        "//utils:time",
    ],
)
