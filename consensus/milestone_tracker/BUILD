cc_library(
    name = "milestone_tracker",
    srcs = ["milestone_tracker.c"],
    hdrs = ["milestone_tracker.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/conf:conf_values",
        "//common:errors",
        "//common/storage:pack",
        "//common/trinary:trit_array",
        "//utils:logger_helper",
        "//utils:time",
        "//utils/handles:thread",
    ],
)
