package(default_visibility = ["//visibility:public"])

cc_library(
    name = "transaction",
    srcs = ["transaction.c"],
    hdrs = ["transaction.h"],
    deps = [
        "//common/curl-p:digest",
        "//common/trinary:trit_long",
        "//common/trinary:trit_array"
    ], )

cc_library(
    name = "tryte_transaction",
    srcs = ["tryte_transaction.cc"],
    hdrs = ["tryte_transaction.h"],
    deps = [
        "//common/model:transaction",
        "//common/trinary:trit_long",
    ],
)

cc_library(
    name="transfer",
    srcs=["transfer.c"],
    hdrs=["transfer.h"],
    deps=[
        "//common/trinary:tryte_long",
        "//common/trinary:trit_tryte",
        "//common/kerl:kerl",
        "//common/helpers:sign",
        "//common/model:transaction"
    ], )

