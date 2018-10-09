cc_library(
    name = "bundle_validator",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//common/model:bundle",
        "//common/sign:normalize",
        "//consensus:defs",
        "//consensus/tangle",
    ],
)
