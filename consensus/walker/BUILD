cc_library(
    name = "walker",
    srcs = ["walker.c"],
    hdrs = ["walker.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
        "//consensus/cw_rating_calculator",
        "//consensus/entry_point_selector",
        "//consensus/milestone",
        "//consensus/tangle",
        "//consensus/walker_validator",
        "//utils:logger_helper",
    ],
)
