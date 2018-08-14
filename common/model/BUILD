package(default_visibility = ["//visibility:public"])

cc_library(
    name = "transaction",
    srcs = ["transaction.c"],
    hdrs = ["transaction.h"],
    deps = [
        "//common/curl-p:digest",
        "//common/trinary:trit_array",
        "//common/trinary:trit_long",
    ],
)

cc_library(
    name = "tryte_transaction",
    srcs = ["tryte_transaction.cc"],
    hdrs = ["tryte_transaction.h"],
    deps = [
        "//common/model:transaction",
        "//common/trinary:trit_array",
        "//common/trinary:trit_long",
    ],
)
