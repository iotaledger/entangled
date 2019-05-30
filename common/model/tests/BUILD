cc_library(
    name = "defs",
    hdrs = ["defs.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/model:transaction",
    ],
)

cc_test(
    name = "test_bundle",
    timeout = "short",
    srcs = ["test_bundle.c"],
    deps = [
        "//common/crypto/iss:normalize",
        "//common/model:bundle",
        "//common/trinary:flex_trit",
        "//common/trinary:tryte_ascii",
        "@unity",
    ],
)

cc_test(
    name = "test_transaction",
    timeout = "short",
    srcs = ["test_transaction.c"],
    deps = [
        ":defs",
        "//common/model:transaction",
        "@unity",
    ],
)

cc_test(
    name = "test_tryte_transaction",
    timeout = "short",
    srcs = ["test_tryte_transaction.cc"],
    deps = [
        ":defs",
        "//common/model:tryte_transaction",
        "//common/trinary:flex_trit",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "test_transfer",
    timeout = "short",
    srcs = ["test_transfer.c"],
    deps = [
        ":defs",
        "//common/model:transfer",
        "//common/trinary:tryte_ascii",
        "//utils:time",
        "@unity",
    ],
)

cc_test(
    name = "test_inputs",
    timeout = "short",
    srcs = ["test_inputs.c"],
    deps = [
        ":defs",
        "//common/model:inputs",
        "@unity",
    ],
)
