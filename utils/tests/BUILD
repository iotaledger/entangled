cc_test(
    name = "test_bundle_miner",
    timeout = "long",
    srcs = ["test_bundle_miner.c"],
    deps =
        [
            "//common/crypto/iss:normalize",
            "//common/trinary:trit_tryte",
            "//common/trinary:trits",
            "//common/trinary:tryte",
            "//utils:bundle_miner",
            "@unity",
        ],
)

cc_test(
    name = "test_merkle",
    timeout = "long",
    srcs = ["test_merkle.c"],
    deps =
        [
            "//common/trinary:trit_tryte",
            "//utils:merkle",
            "@unity",
        ],
)
