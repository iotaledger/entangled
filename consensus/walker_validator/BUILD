cc_library(
    name = "walker_validator",
    srcs = ["walker_validator.c"],
    hdrs = ["walker_validator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
        "//consensus/entry_point_selector",
        "//consensus/ledger_validator",
        "//consensus/milestone",
        "//consensus/tangle",
        "//utils:logger_helper",
    ],
)
