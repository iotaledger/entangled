cc_test(
    name = "test_mam",
    srcs = ["test_mam.c"],
    linkopts = ["-lpthread"],
    deps =
        [
            "//common/trinary:trit_tryte",
            "//mam/v1:mam",
            "@unity",
        ],
)

cc_test(
    name = "test_mask",
    srcs = ["test_mask.c"],
    deps =
        [
            "//common/trinary:trit_tryte",
            "//mam/v1:mask",
            "@unity",
        ],
)

cc_test(
    name = "test_merkle",
    srcs = ["test_merkle.c"],
    deps =
        [
            "//common/trinary:trit_tryte",
            "//mam/v1:merkle",
            "@unity",
        ],
)
