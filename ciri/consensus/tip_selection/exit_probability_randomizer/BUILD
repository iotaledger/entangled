cc_library(
    name = "global_calcs",
    srcs = ["global_calcs.c"],
    hdrs = ["global_calcs.h"],
    deps = [
        "//ciri/consensus/tip_selection/cw_rating_calculator",
        "//common:errors",
        "//common/trinary:flex_trit",
        "//utils/handles:rand",
    ],
)

cc_library(
    name = "shared",
    visibility = ["//visibility:public"],
    deps = [
        ":global_calcs",
        "//ciri/consensus/tangle",
        "//ciri/consensus/tip_selection/cw_rating_calculator",
        "//ciri/consensus/tip_selection/exit_probability_validator",
        "//common:errors",
        "//common/trinary:flex_trit",
        "//utils:logger_helper",
        "//utils/containers/hash:hash_double_map",
        "//utils/handles:rand",
    ],
)

cc_library(
    name = "exit_probability_randomizer",
    srcs = ["exit_probability_randomizer.c"],
    visibility = ["//visibility:public"],
    deps = [
        ":exit_prob_map",
        ":walker",
    ],
)

cc_library(
    name = "walker",
    srcs = ["walker.c"],
    hdrs = [
        "exit_probability_randomizer.h",
        "walker.h",
    ],
    visibility = ["//visibility:public"],
    deps = [":shared"],
)

cc_library(
    name = "exit_prob_map",
    srcs = ["exit_prob_map.c"],
    hdrs = [
        "exit_prob_map.h",
        "exit_probability_randomizer.h",
    ],
    visibility = ["//visibility:public"],
    deps = [":shared"],
)
