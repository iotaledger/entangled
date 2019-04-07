package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ftroika",
    srcs = glob(["*.c"]) + glob(["*.h"]),
    hdrs = ["ftroika.h"],
    deps = [
        ":t27",
        "//common:stdint",
        "//common/trinary:trits",
        "//common/trinary:tryte",
    ],
)

cc_library(
    name = "t27",
    srcs = ["t27.h"],
    deps = [
        "//common:stdint",
        "//common/trinary:trits",
        "//common/trinary:tryte",
    ],
)
