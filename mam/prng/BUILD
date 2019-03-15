package(default_visibility = ["//visibility:public"])

cc_library(
    name = "prng",
    srcs = ["prng.c"],
    hdrs = ["prng.h"],
    deps = [
        "//mam:defs",
        "//mam/pb3",
        "//mam/sponge",
        "//mam/trits",
        "//utils:memset_safe",
    ],
)
