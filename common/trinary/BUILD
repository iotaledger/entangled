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

cc_library(
    name = "trit_array",
    srcs = ["trit_array.c"],
    hdrs = ["trit_array.h"],
    deps = [
        ":trit_byte",
        ":trits",
        "//common:stdint",
    ],
)

cc_library(
    name = "trit_array_1",
    srcs = ["trit_array.c"],
    hdrs = ["trit_array.h"],
    copts = ["-DTRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE"],
    deps = [
        ":trit_byte",
        ":trits",
        "//common:stdint",
    ],
)

cc_library(
    name = "trit_array_4",
    srcs = ["trit_array.c"],
    hdrs = ["trit_array.h"],
    copts = ["-DTRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE"],
    deps = [
        ":trit_byte",
        ":trits",
        "//common:stdint",
    ],
)

cc_library(
    name = "trit_array_5",
    srcs = ["trit_array.c"],
    hdrs = ["trit_array.h"],
    copts = ["-DTRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE"],
    deps = [
        ":trit_byte",
        ":trits",
        "//common:stdint",
    ],
)
