package(default_visibility = ["//visibility:public"])

load(":defs.bzl", "TRINARY_INT_1_TRITS_PER_WORD_DEFS")
load(":defs.bzl", "TRINARY_INTERLEAVED_1_TRITS_PER_WORD_DEFS")
load(":defs.bzl", "TRINARY_PACKED_1_TRITS_PER_WORD_DEFS")

cc_library(
    name = "mam",
    srcs = ["mam.c"],
    hdrs = ["mam.h"],
    deps = [
        ":defs",
        ":list",
        "//mam/v2/ntru",
        "//mam/v2/pb3",
        "//mam/v2/prng",
    ],
)

cc_library(
    name = "defs",
    hdrs = ["defs.h"],
    defines = select({
        "//conditions:default": TRINARY_INT_1_TRITS_PER_WORD_DEFS,
    }),
    deps = [
        "//common/trinary:trit_long",
        "//common/trinary:trits",
    ],
)

cc_library(
    name = "list",
    srcs = ["list.h"],
    hdrs = ["list.h"],
)

cc_library(
    name = "err",
    hdrs = ["err.h"],
)
