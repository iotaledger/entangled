cc_library(
    name = "tip_selector",
    srcs = ["tip_selector.c"],
    hdrs = ["tip_selector.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
        "//consensus:model",
        "//consensus/ledger_validator",
        "//consensus/milestone:milestone_tracker",
        "//consensus/tangle",
        "//consensus/tip_selection/cw_rating_calculator",
        "//consensus/tip_selection/entry_point_selector",
        "//consensus/tip_selection/exit_probability_randomizer",
        "//consensus/tip_selection/exit_probability_validator",
        "//utils:logger_helper",
    ],
)
