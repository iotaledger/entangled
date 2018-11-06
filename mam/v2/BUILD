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
        ":errs",
        ":list",
        ":ntru",
        ":pb3",
        ":prng",
    ],
)

cc_library(
    name = "defs",
    hdrs = ["defs.h"],
    defines = select({
        ":1_trits_per_word": TRINARY_INT_1_TRITS_PER_WORD_DEFS,
        "//conditions:default": TRINARY_INT_1_TRITS_PER_WORD_DEFS,
    }),
)

cc_library(
    name = "list",
    srcs = ["list.h"],
    hdrs = ["list.h"],
)

cc_library(
    name = "errs",
    srcs = ["err.c"],
    hdrs = ["err.h"],
)

cc_library(
    name = "alloc",
    srcs = ["alloc.c"],
    hdrs = ["alloc.h"],
    deps = [":defs"],
)

cc_library(
    name = "buffers",
    srcs = ["buffers.c"],
    hdrs = ["buffers.h"],
    deps = [
        "trits",
        ":defs",
    ],
)

cc_library(
    name = "trits",
    srcs = ["trits.c"],
    hdrs = ["trits.h"],
    deps = [
        ":alloc",
        ":defs",
    ],
)

cc_library(
    name = "prng",
    srcs = ["prng.c"],
    hdrs = ["prng.h"],
    deps = [
        ":defs",
        ":errs",
        ":sponge",
        ":trits",
    ],
)

cc_library(
    name = "sponge",
    srcs = ["sponge.c"],
    hdrs = ["sponge.h"],
    deps = [
        ":buffers",
        ":defs",
        ":trits",
    ],
)

cc_library(
    name = "wots",
    srcs = ["wots.c"],
    hdrs = ["wots.h"],
    deps = [
        ":prng",
        ":sponge",
    ],
)

cc_library(
    name = "mss",
    srcs = ["mss.c"],
    hdrs = ["mss.h"],
    deps = [
        ":wots",
    ],
)

cc_library(
    name = "ntru",
    srcs = ["ntru.c"],
    hdrs = ["ntru.h"],
    deps = [
        ":prng",
        ":sponge",
    ],
)

cc_library(
    name = "pb3",
    srcs = ["pb3.c"],
    hdrs = ["pb3.h"],
    deps = [
        ":mss",
    ],
)

cc_library(
    name = "curl",
    srcs = ["curl.c"],
    hdrs = ["curl.h"],
    deps = [
        ":defs",
        ":sponge",
        ":trits",
    ],
)

config_setting(
    name = "1_trits_per_word",
    values = {"define": "trits_per_word=1"},
)
