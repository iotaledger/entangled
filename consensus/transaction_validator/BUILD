cc_library(
    name = "transaction_validator",
    srcs = ["transaction_validator.c"],
    hdrs = ["transaction_validator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
        "//consensus/milestone",
        "//consensus/tangle",
        "//utils:logger_helper",
    ],
)
