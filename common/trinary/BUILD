package(default_visibility=["//visibility:public"])

cc_library(
    name="bytes",
    hdrs=["bytes.h"],
    deps=[
        "//common:stdint",
    ], )

cc_library(
    name="trits",
    hdrs=["trits.h"], )

cc_library(
    name="tryte",
    hdrs=["tryte.h"],
    deps=[
        "//common:stdint",
    ], )

cc_library(
    name="bct",
    srcs=["bct.c"],
    hdrs=["bct.h"],
    deps=[":trits"], )

cc_library(
    name="ptrits",
    srcs=[
        "ptrit_incr.c",
    ],
    hdrs=[
        "ptrit.h",
        "ptrit_incr.h",
    ],
    deps=[
        "//common:stdint",
    ])

cc_library(
    name="trit_byte",
    srcs=["trit_byte.c"],
    hdrs=["trit_byte.h"],
    deps=[
        ":trits",
        ":bytes",
    ])

cc_library(
    name="trit_tryte",
    srcs=["trit_tryte.c"],
    hdrs=["trit_tryte.h"],
    deps=[
        ":trits",
        ":tryte",
    ])

cc_library(
    name="trit_long",
    srcs=["trit_long.c"],
    hdrs=["trit_long.h"],
    deps=[
        ":trits",
        "//common:stdint",
    ])

cc_library(
    name="trit_ptrit",
    srcs=["trit_ptrit.c"],
    hdrs=["trit_ptrit.h"],
    deps=[
        ":trits",
        ":ptrits",
        "//common:stdint",
    ])
