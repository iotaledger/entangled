cc_library(
    name = "ledger_validator",
    srcs = ["ledger_validator.c"],
    hdrs = ["ledger_validator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
        "//consensus:model",
        "//consensus/entry_point_selector",
        "//consensus/tangle",
        "//utils:logger_helper",
    ],
)
