cc_test(
    name = "test_iss",
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
    srcs = [
        "test_iss_kerl.c",
    ],
    deps = [
        "//common/crypto/iss/v1:iss_kerl",
        "//common/trinary:trit_tryte",
        "@unity",
    ],
)
