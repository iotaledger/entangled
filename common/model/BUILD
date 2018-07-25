package(default_visibility = ["//visibility:public"])

cc_library(
    name="transaction",
    srcs=["transaction.c"],
    hdrs=["transaction.h"],
    deps=[
        "//common/trinary:trit_long",
        "//common/trinary:trit_array"
    ], )

cc_library(
    name="transaction_3",
    srcs=["transaction.c"],
    hdrs=["transaction.h"],
    defines = ["TRIT_ARRAY_ENCODING_3_TRITS_PER_BYTE"],
    deps=[
        "//common/trinary:trit_long",
        "//common/trinary:trit_array_3"
    ], )

cc_library(
    name="transaction_4",
    srcs=["transaction.c"],
    hdrs=["transaction.h"],
    defines = ["TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE"],
    deps=[
        "//common/trinary:trit_long",
        "//common/trinary:trit_array_4"
    ], )

cc_library(
    name="transaction_5",
    srcs=["transaction.c"],
    hdrs=["transaction.h"],
    defines = ["TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE"],
    deps=[
        "//common/trinary:trit_long",
        "//common/trinary:trit_array_5"
    ], )

cc_library(
    name="cpptransaction",
    srcs=["cpptransaction.cc"],
    hdrs=["cpptransaction.h"],
    deps=[
        "//common/model:transaction",
        "//common/trinary:trit_long",
        "//common/trinary:trit_array"
    ], )

cc_library(
    name="cpptransaction_3",
    srcs=["cpptransaction.cc"],
    hdrs=["cpptransaction.h"],
    defines = ["TRIT_ARRAY_ENCODING_3_TRITS_PER_BYTE"],
    deps=[
        "//common/model:transaction_3",
        "//common/trinary:trit_long",
        "//common/trinary:trit_array_3"
    ], )

cc_library(
    name="cpptransaction_4",
    srcs=["cpptransaction.cc"],
    hdrs=["cpptransaction.h"],
    defines = ["TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE"],
    deps=[
        "//common/model:transaction_4",
        "//common/trinary:trit_long",
        "//common/trinary:trit_array_4"
    ], )

cc_library(
    name="cpptransaction_5",
    srcs=["cpptransaction.cc"],
    hdrs=["cpptransaction.h"],
    defines = ["TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE"],
    deps=[
        "//common/model:transaction_5",
        "//common/trinary:trit_long",
        "//common/trinary:trit_array_5"
    ], )
