cc_library(
    name = "snapshot",
    srcs = ["snapshot.c"],
    hdrs = ["snapshot.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
        "//utils:logger_helper",
        "//utils/handles:rw_lock",
        "@com_github_uthash//:uthash",
    ],
)
