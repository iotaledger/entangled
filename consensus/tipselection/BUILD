cc_library(
    name = "tipselection",
    srcs = ["tipselection.c"],
    hdrs = ["tipselection.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
        "//consensus:model",
        "//consensus/cw_rating_calculator",
        "//consensus/entry_point_selector",
        "//consensus/exit_probability_randomizer",
        "//consensus/exit_probability_validator",
        "//consensus/ledger_validator",
        "//utils:logger_helper",
    ],
)
