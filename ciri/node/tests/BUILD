cc_test(
    name = "test_tips_cache",
    timeout = "short",
    srcs = ["test_tips_cache.c"],
    deps = [
        "//ciri/node:tips_cache",
        "@unity",
    ],
)

cc_test(
    name = "test_recent_seen_bytes_cache",
    timeout = "short",
    srcs = ["test_recent_seen_bytes_cache.c"],
    deps = [
        "//ciri/consensus/test_utils",
        "//ciri/node:recent_seen_bytes_cache",
        "@unity",
    ],
)
