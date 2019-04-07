cc_library(
    name = "bundle_validator",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//common/crypto/iss:normalize",
        "//common/model:bundle",
        "//consensus:conf",
        "//consensus/tangle",
    ],
)
