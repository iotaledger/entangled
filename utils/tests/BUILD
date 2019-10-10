load("//ciri/consensus:conf.bzl", "CONSENSUS_MAINNET_VARIABLES")

cc_test(
    name = "test_bundle_miner",
    timeout = "short",
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
    timeout = "moderate",
    srcs = ["test_merkle.c"],
    deps =
        [
            "//common/trinary:trit_tryte",
            "//utils:merkle",
            "@unity",
        ],
)

cc_test(
    name = "test_signed_files",
    timeout = "long",
    srcs = ["test_signed_files.c"],
    data = [
        "fake.sig",
        "//ciri/consensus/snapshot:snapshot_files",
    ],
    defines = CONSENSUS_MAINNET_VARIABLES,
    deps = [
        "//ciri/consensus/snapshot",
        "//utils:signed_files",
        "@unity",
    ],
)
