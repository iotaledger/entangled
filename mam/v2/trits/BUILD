package(default_visibility = ["//visibility:public"])

cc_library(
    name = "buffers",
    srcs = ["buffers.c"],
    hdrs = ["buffers.h"],
    deps = [
        ":trits",
        "//mam/v2:defs",
    ],
)

cc_library(
    name = "trits",
    srcs = ["trits.c"],
    hdrs = ["trits.h"],
    deps = [
        "//common/trinary:trit_tryte",
        "//mam/v2:defs",
    ],
)
