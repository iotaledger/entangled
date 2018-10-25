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
    timeout = "moderate",
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
