cc_library(
    name = "tip_selector",
    srcs = ["tip_selector.c"],
    hdrs = ["tip_selector.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus:model",
        "//ciri/consensus/ledger_validator",
        "//ciri/consensus/milestone:milestone_tracker",
        "//ciri/consensus/tangle",
        "//ciri/consensus/tip_selection/cw_rating_calculator",
        "//ciri/consensus/tip_selection/entry_point_selector",
        "//ciri/consensus/tip_selection/exit_probability_randomizer",
        "//ciri/consensus/tip_selection/exit_probability_validator",
        "//common:errors",
        "//utils:logger_helper",
    ],
)
