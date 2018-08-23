cc_library(
    name = "cw_rating_calculator",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
        "//consensus/tangle",
        "//utils:logger_helper",
        "@com_github_uthash//:uthash",
    ],
)
