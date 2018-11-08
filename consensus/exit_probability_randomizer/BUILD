cc_library(
    name = "exit_probability_randomizer",
    visibility = ["//visibility:public"],
    deps = [
        ":walker",
    ],
)

cc_library(
    name = "walker",
    srcs = [
        "exit_probability_randomizer.c",
        "walker.c",
    ],
    hdrs = [
        "exit_probability_randomizer.h",
        "walker.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
        "//consensus/cw_rating_calculator",
        "//consensus/exit_probability_validator",
        "//consensus/tangle",
        "//utils:logger_helper",
        "//utils/handles:rand",
    ],
)
