cc_test(
    name = "test_merkle",
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
    srcs = ["test_signed_files.c"],
    data = [
        "fake.sig",
        "//consensus/snapshot:snapshot_files",
    ],
    deps = [
        "//consensus/snapshot",
        "//utils:signed_files",
        "@unity",
    ],
)

cc_test(
    name = "test_hash_containers",
    srcs = [
        "test_hash_containers.c",
        "test_hash_containers.h",
    ],
    deps = [
        "//common/model:transaction",
        "//common/trinary:flex_trit",
        "//utils:hash_containers",
        "@unity",
    ],
)
