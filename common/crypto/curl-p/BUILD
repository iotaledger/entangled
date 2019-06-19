package(default_visibility = ["//visibility:public"])

cc_library(
    name = "curl-p-const",
    srcs = ["const.c"],
    hdrs = [
        "const.h",
        "indices.h",
    ],
    deps = [
        "//common:defs",
        "//common/trinary:trits",
    ],
)

cc_library(
    name = "trit",
    srcs = ["curl_p.c"],
    hdrs = [
        "indices.h",
        "trit.h",
    ],
    deps = [
        ":curl-p-const",
        "//common:stdint",
        "//common/trinary:trits",
        "//utils:forced_inline",
        "//utils:memset_safe",
    ],
)

cc_library(
    name = "ptrit",
    srcs = ["ptrit.c"],
    hdrs = [
        "ptrit.h",
    ],
    deps = [
        ":curl-p-const",
        "//common:stdint",
        "//common/trinary:ptrits",
        "//common/trinary:trits",
        "//utils:forced_inline",
        "//utils:memset_safe",
    ],
)

cc_library(
    name = "search",
    hdrs = [
        "search.h",
    ],
    deps = [
        ":curl-p-const",
        ":ptrit",
        ":trit",
        "//common:stdint",
        "//common/trinary:ptrits",
        "//common/trinary:trit_ptrit",
        "//common/trinary:trits",
    ],
)

cc_library(
    name = "pearl_diver",
    srcs = [
        "pearl_diver.c",
    ],
    hdrs = [
        "pearl_diver.h",
    ],
    deps = [
        ":curl-p-const",
        ":ptrit",
        ":search",
        ":trit",
        "//common:stdint",
        "//common/trinary:ptrits",
        "//common/trinary:trit_ptrit",
        "//common/trinary:trits",
        "//utils:system",
        "//utils/handles:rw_lock",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "hashcash",
    srcs = [
        "hashcash.c",
    ],
    hdrs = [
        "hashcash.h",
    ],
    deps = [
        ":curl-p-const",
        ":pearl_diver",
    ],
)

cc_library(
    name = "hamming",
    srcs = [
        "hamming.c",
    ],
    hdrs = [
        "hamming.h",
    ],
    deps = [
        ":curl-p-const",
        ":pearl_diver",
    ],
)

cc_library(
    name = "digest",
    srcs = ["digest.c"],
    hdrs = ["digest.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":trit",
        "//common/trinary:trit_tryte",
    ],
)
