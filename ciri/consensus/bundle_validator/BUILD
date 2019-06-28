cc_library(
    name = "bundle_validator",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus:conf",
        "//ciri/consensus/tangle",
        "//common/crypto/iss:normalize",
        "//common/model:bundle",
    ],
)
