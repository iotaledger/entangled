cc_test(
    name = "test_curlp",
    timeout = "short",
    srcs = ["test_curlp.c"],
    deps = [
        "//common/crypto/curl-p:trit",
        "@unity",
    ],
)

cc_test(
    name = "test_curlp_ptrit",
    timeout = "short",
    srcs = [
        "test_curlp_ptrit.c",
        "test_curlp_ptrit.h",
    ],
    deps = [
        "//common/crypto/curl-p:ptrit",
        "//common/crypto/curl-p:trit",
        "@unity",
    ],
)

cc_test(
    name = "test_cpu_hashcash",
    timeout = "short",
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
    timeout = "short",
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
