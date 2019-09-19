cc_library(
    name = "exit_probability_validator",
    srcs = ["exit_probability_validator.c"],
    hdrs = ["exit_probability_validator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus:model",
        "//ciri/consensus/ledger_validator",
        "//ciri/consensus/tangle",
        "//ciri/consensus/tip_selection/entry_point_selector",
        "//ciri/consensus/transaction_solidifier",
        "//common:errors",
        "//utils:hash_maps",
        "//utils:logger_helper",
        "@com_github_uthash//:uthash",
    ],
)
