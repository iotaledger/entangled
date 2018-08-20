package(default_visibility = ["//visibility:public"])

cc_library(
    name = "bytes",
    hdrs = ["bytes.h"],
    deps = [
        "//common:stdint",
    ],
)

cc_library(
    name = "trits",
    hdrs = ["trits.h"],
    deps = [
        "//common:stdint",
    ],
)

cc_library(
    name = "tryte",
    hdrs = ["tryte.h"],
    deps = [
        "//common:stdint",
    ],
)

cc_library(
    name = "bct",
    srcs = ["bct.c"],
    hdrs = ["bct.h"],
    deps = [":trits"],
)

cc_library(
    name = "ptrits",
    srcs = [
        "ptrit_incr.c",
    ],
    hdrs = [
        "ptrit.h",
        "ptrit_incr.h",
    ],
    deps = [
        "//common:stdint",
    ],
)

cc_library(
    name = "trit_byte",
    srcs = ["trit_byte.c"],
    hdrs = ["trit_byte.h"],
    deps = [
        ":bytes",
        ":trits",
    ],
)

cc_library(
    name = "trit_tryte",
    srcs = ["trit_tryte.c"],
    hdrs = ["trit_tryte.h"],
    deps = [
        ":trits",
        ":tryte",
    ],
)

cc_library(
    name = "trit_long",
    srcs = ["trit_long.c"],
    hdrs = ["trit_long.h"],
    deps = [
        ":trits",
        "//common:stdint",
    ],
)

cc_library(
    name = "trit_ptrit",
    srcs = ["trit_ptrit.c"],
    hdrs = ["trit_ptrit.h"],
    deps = [
        ":ptrits",
        ":trits",
        "//common:stdint",
    ],
)

cc_library(
    name = "add",
    srcs = ["add.c"],
    hdrs = ["add.h"],
    deps = [
        ":trits",
    ],
)

cc_library(
    name = "tryte_long",
    srcs = ["tryte_long.c"],
    hdrs = ["tryte_long.h"],
    deps = [
        ":tryte",
    ],
)

config_setting(
    name = "trit_encoding_1",
    values = {"define": "trit_encoding=1"},
)

config_setting(
    name = "trit_encoding_3",
    values = {"define": "trit_encoding=3"},
)

config_setting(
    name = "trit_encoding_4",
    values = {"define": "trit_encoding=4"},
)

config_setting(
    name = "trit_encoding_5",
    values = {"define": "trit_encoding=5"},
)

cc_library(
    name = "trit_array",
    deps = select({
        ":trit_encoding_1": [":trit_array_1"],
        ":trit_encoding_3": [":trit_array_3"],
        ":trit_encoding_4": [":trit_array_4"],
        ":trit_encoding_5": [":trit_array_5"],
        "//conditions:default": [":trit_array_3"],
    }),
)

cc_library(
    name = "trit_array_1",
    srcs = ["trit_array.c"],
    hdrs = ["trit_array.h"],
    defines = ["TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE"],
    deps = [
        ":trit_byte",
        ":trit_tryte",
        ":trits",
        "//common:stdint",
    ],
)

cc_library(
    name = "trit_array_3",
    srcs = ["trit_array.c"],
    hdrs = ["trit_array.h"],
    defines = ["TRIT_ARRAY_ENCODING_3_TRITS_PER_BYTE"],
    deps = [
        ":trit_byte",
        ":trits",
        ":trit_tryte",
        "//common:stdint",
    ],
)

cc_library(
    name = "trit_array_4",
    srcs = ["trit_array.c"],
    hdrs = ["trit_array.h"],
    defines = ["TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE"],
    deps = [
        ":trit_byte",
        ":trit_tryte",
        ":trits",
        "//common:stdint",
    ],
)

cc_library(
    name = "trit_array_5",
    srcs = ["trit_array.c"],
    hdrs = ["trit_array.h"],
    defines = ["TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE"],
    deps = [
        ":trit_byte",
        ":trit_tryte",
        ":trits",
        "//common:stdint",
    ],
)

cc_library(
    name = "flex_trit_array",
    srcs = ["flex_trit_array.cc"],
    hdrs = ["flex_trit_array.h"],
    deps = [
        ":trit_array",
        "//common:stdint",
    ],
)

cc_library(
    name = "flex_trit_array_3",
    srcs = ["flex_trit_array.cc"],
    hdrs = ["flex_trit_array.h"],
    defines = ["TRIT_ARRAY_ENCODING_3_TRITS_PER_BYTE"],
    deps = [
        ":trit_array_3",
        "//common:stdint",
    ],
)

cc_library(
    name = "flex_trit_array_4",
    srcs = ["flex_trit_array.cc"],
    hdrs = ["flex_trit_array.h"],
    defines = ["TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE"],
    deps = [
        ":trit_array_4",
        "//common:stdint",
    ],
)

cc_library(
    name = "flex_trit_array_5",
    srcs = ["flex_trit_array.cc"],
    hdrs = ["flex_trit_array.h"],
    defines = ["TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE"],
    deps = [
        ":trit_array_5",
        "//common:stdint",
    ],
)
