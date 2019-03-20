package(default_visibility = ["//visibility:public"])

cc_library(
    name = "bundle",
    srcs = ["bundle.c"],
    hdrs = ["bundle.h"],
    deps = [
        ":transaction",
        "//common:errors",
        "//common/crypto/iss:normalize",
        "//common/crypto/iss/v1:iss_kerl",
        "//common/model:transfer",
        "//common/trinary:flex_trit",
        "//common/trinary:trit_tryte",
        "//common/trinary:tryte_long",
        "@com_github_uthash//:uthash",
    ],
)

cc_library(
    name = "milestone",
    hdrs = ["milestone.h"],
    deps = ["//common/trinary:flex_trit"],
)

cc_library(
    name = "transaction",
    srcs = ["transaction.c"],
    hdrs = ["transaction.h"],
    deps = [
        "//common/crypto/curl-p:digest",
        "//common/trinary:flex_trit",
        "//common/trinary:trit_long",
        "@com_github_uthash//:uthash",
    ],
)

cc_library(
    name = "transfer",
    srcs = ["transfer.c"],
    hdrs = ["transfer.h"],
    deps = [
        "//common/crypto/kerl",
        "//common/helpers:sign",
        "//common/model:transaction",
        "//utils:logger_helper",
    ],
)

cc_library(
    name = "tryte_transaction",
    srcs = ["tryte_transaction.cc"],
    hdrs = ["tryte_transaction.h"],
    deps = [
        "//common/model:transaction",
    ],
)
