cc_test(
    name = "test_transaction",
    srcs = ["test_transaction.c"],
    deps = [
        "//common/model:transaction",
        "@unity",
    ],
)

cc_test(
    name = "test_tryte_transaction",
    srcs = ["test_tryte_transaction.cc"],
    deps = [
        "//common/model:tryte_transaction",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test (
    name = "test_transfer",
    srcs = ["test_transfer.c"],
    deps = [
        "//common/model:transfer",
        "@unity//:unity",
        ]
)
