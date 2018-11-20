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
        ":ntru",
        ":pb3",
        ":prng",
    ],
)

cc_library(
    name = "defs",
    hdrs = ["defs.h"],
    defines = select({
        "//conditions:default": TRINARY_INT_1_TRITS_PER_WORD_DEFS,
    }),
    deps = [
        "//common/trinary:trit_array",
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
    name = "buffers",
    srcs = ["buffers.c"],
    hdrs = ["buffers.h"],
    deps = [
        "trits",
        ":defs",
        "//common/trinary:trit_array",
    ],
)

cc_library(
    name = "trits",
    srcs = ["trits.c"],
    hdrs = ["trits.h"],
    deps = [
        ":defs",
        "//common/trinary:flex_trit",
        "//common/trinary:trit_tryte",
    ],
)

cc_library(
    name = "prng",
    srcs = ["prng.c"],
    hdrs = ["prng.h"],
    deps = [
        ":defs",
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
        "//common/trinary:add",
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
        "//common:errors",
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
        "//common:errors",
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
