package(default_visibility = ["//visibility:public"])

cc_library(
    name="transaction_1",
    srcs=["transaction.c"],
    hdrs=["transaction.h"],
    copts = ["-DTRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE"],
    deps=[
        "//common/trinary:trit_long",
        "//common/trinary:trit_array_1"
    ], )

cc_library(
    name="transaction_4",
    srcs=["transaction.c"],
    hdrs=["transaction.h"],
    copts = ["-DTRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE"],
    deps=[
        "//common/trinary:trit_long",
        "//common/trinary:trit_array_4"
    ], )

cc_library(
    name="transaction_5",
    srcs=["transaction.c"],
    hdrs=["transaction.h"],
    copts = ["-DTRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE"],
    deps=[
        "//common/trinary:trit_long",
        "//common/trinary:trit_array_5"
    ], )
