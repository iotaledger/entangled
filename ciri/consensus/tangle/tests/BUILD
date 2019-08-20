cc_test(
    name = "test_tangle",
    timeout = "moderate",
    srcs = ["test_tangle.c"],
    data = ["//common/storage:tangle_db"],
    deps = [
        "//ciri/consensus/tangle",
        "//ciri/consensus/test_utils",
        "@unity",
    ],
)
