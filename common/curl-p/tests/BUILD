cc_test(
    name = "test_curlp",
    srcs = ["test_curlp.c"],
    deps = [
        "//common/curl-p:trit",
        "@unity",
    ],
)

cc_test(
    name = "test_curlp_bct",
    srcs = ["test_bct.c"],
    deps = [
        "//common/curl-p:bct",
        "@unity",
    ],
)

cc_test(
    name = "test_curlp_ptrit",
    srcs = [
        "test_curlp_ptrit.c",
        "test_curlp_ptrit.h",
    ],
    deps = [
        "//common/curl-p:ptrit",
        "@unity",
    ],
)

cc_test(
    name = "test_cpu_hashcash",
    srcs = [
        "test_cpu_hashcash.c",
    ],
    linkopts = ["-lpthread"],
    tags = ["exclusive"],
    deps = [
        "//common/curl-p:hashcash",
        "@unity",
    ],
)

cc_test(
    name = "test_cpu_hamming",
    srcs = [
        "test_cpu_hamming.c",
    ],
    linkopts = ["-lpthread"],
    tags = ["exclusive"],
    deps = [
        "//common/curl-p:hamming",
        "@unity",
    ],
)
