package(default_visibility = ["//visibility:public"])

load(":defs.bzl", "TRINARY_INT_1_TRITS_PER_WORD_DEFS")
load(":defs.bzl", "TRINARY_INTERLEAVED_1_TRITS_PER_WORD_DEFS")
load(":defs.bzl", "TRINARY_PACKED_1_TRITS_PER_WORD_DEFS")

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
    name = "alloc",
    srcs = ["alloc.c"],
    hdrs = ["alloc.h"],
    deps = [":defs"],
)

cc_library(
    name = "err",
    hdrs = ["err.h"],
)

cc_library(
    name = "list",
    hdrs = ["list.h"],
)
