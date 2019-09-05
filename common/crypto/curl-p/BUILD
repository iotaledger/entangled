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

config_setting(
    name = "pcurl_state_double",
    values = {"define": "pcurl_state=double"},
)

config_setting(
    name = "pcurl_state_short",
    values = {"define": "pcurl_state=short"},
)

config_setting(
    name = "pcurl_sbox_unwind_1",
    values = {"define": "pcurl_sbox_unwind=1"},
)

config_setting(
    name = "pcurl_sbox_unwind_2",
    values = {"define": "pcurl_sbox_unwind=2"},
)

config_setting(
    name = "pcurl_sbox_unwind_4",
    values = {"define": "pcurl_sbox_unwind=4"},
)

config_setting(
    name = "pcurl_sbox_unwind_8",
    values = {"define": "pcurl_sbox_unwind=8"},
)

cc_library(
    name = "ptrit",
    srcs = ["ptrit.c"],
    hdrs = [
        "ptrit.h",
    ],
    defines = select({
        ":pcurl_state_double": ["PCURL_STATE_DOUBLE"],
        ":pcurl_state_short": ["PCURL_STATE_SHORT"],
        "//conditions:default": ["PCURL_STATE_SHORT"],
    }) + select({
        ":pcurl_sbox_unwind_1": ["PCURL_SBOX_UNWIND_1"],
        ":pcurl_sbox_unwind_2": ["PCURL_SBOX_UNWIND_2"],
        ":pcurl_sbox_unwind_4": ["PCURL_SBOX_UNWIND_4"],
        ":pcurl_sbox_unwind_8": ["PCURL_SBOX_UNWIND_8"],
        "//conditions:default": ["PCURL_SBOX_UNWIND_2"],
    }),
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
