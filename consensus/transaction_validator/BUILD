cc_library(
    name = "transaction_validator",
    srcs = ["transaction_validator.c"],
    hdrs = ["transaction_validator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:transaction",
        "//consensus:conf",
        "//utils:logger_helper",
        "//utils:time",
    ],
)
