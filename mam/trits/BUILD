package(default_visibility = ["//visibility:public"])

cc_library(
    name = "buffers",
    srcs = ["buffers.c"],
    hdrs = ["buffers.h"],
    deps = [
        ":trits",
        "//mam:defs",
    ],
)

cc_library(
    name = "trits",
    srcs = ["trits.c"],
    hdrs = ["trits.h"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_long",
        "//common/trinary:trit_tryte",
        "//mam:defs",
    ],
)
