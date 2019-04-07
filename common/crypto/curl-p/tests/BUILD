cc_test(
    name = "test_curlp",
    srcs = ["test_curlp.c"],
    deps = [
        "//common/crypto/curl-p:trit",
        "@unity",
    ],
)

cc_test(
    name = "test_curlp_bct",
    srcs = ["test_bct.c"],
    deps = [
        "//common/crypto/curl-p:bct",
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
        "//common/crypto/curl-p:ptrit",
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
        "//common/crypto/curl-p:hashcash",
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
        "//common/crypto/curl-p:hamming",
        "@unity",
    ],
)
