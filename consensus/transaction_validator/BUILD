cc_library(
    name = "transaction_validator",
    srcs = ["transaction_validator.c"],
    hdrs = ["transaction_validator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:transaction",
        "//consensus:conf",
        "//consensus/snapshot:snapshots_provider",
        "//node/components:transaction_requester_shared",
        "//utils:logger_helper",
        "//utils:time",
    ],
)
