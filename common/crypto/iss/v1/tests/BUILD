cc_test(
    name = "test_iss",
    timeout = "moderate",
    srcs = [
        "test_iss.c",
    ],
    deps = [
        "//common/crypto/iss:normalize",
        "//common/crypto/iss/v1:iss",
        "//common/trinary:trit_tryte",
        "@unity",
    ],
)

cc_test(
    name = "test_iss_curl",
    timeout = "short",
    srcs = [
        "test_iss_curl.c",
    ],
    deps = [
        "//common/crypto/iss/v1:iss_curl",
        "//common/trinary:trit_tryte",
        "@unity",
    ],
)

cc_test(
    name = "test_iss_kerl",
    timeout = "short",
    srcs = [
        "test_iss_kerl.c",
    ],
    deps = [
        "//common/crypto/iss/v1:iss_kerl",
        "//common/trinary:trit_tryte",
        "@unity",
    ],
)
