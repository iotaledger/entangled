cc_test(
    name = "test_mam",
    timeout = "long",
    srcs = ["test_mam.c"],
    linkopts = ["-lpthread"],
    deps =
        [
            "//common/trinary:trit_tryte",
            "//mam/prototype:mam",
            "@unity",
        ],
)

cc_test(
    name = "test_mask",
    timeout = "short",
    srcs = ["test_mask.c"],
    deps =
        [
            "//common/trinary:trit_tryte",
            "//mam/prototype:mask",
            "@unity",
        ],
)
