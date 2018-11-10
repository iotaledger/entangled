cc_library(
    name = "structs",
    hdrs = ["common.h"],
    deps = ["//mam/v2:mam"],
)

cc_library(
    name = "shared",
    deps =
        [
            ":structs",
            "//mam/v2:curl",
            "//mam/v2:mam",
            "//mam/v2:trits",
            "@unity",
        ],
)

cc_test(
    name = "test_trits",
    srcs = [
        "test_trits.c",
    ],
    deps =
        [
            ":shared",
        ],
)

cc_test(
    name = "test_curl",
    srcs = [
        "test_curl.c",
    ],
    deps =
        [
            ":shared",
        ],
)

cc_test(
    name = "test_sponge",
    srcs = [
        "test_sponge.c",
    ],
    deps =
        [
            ":shared",
        ],
)

cc_test(
    name = "test_prng",
    srcs = [
        "test_prng.c",
    ],
    deps =
        [
            ":shared",
        ],
)

cc_test(
    name = "test_wots",
    srcs = [
        "test_wots.c",
    ],
    deps =
        [
            ":shared",
        ],
)

cc_test(
    name = "test_mss",
    timeout = "moderate",
    srcs = [
        "test_mss.c",
    ],
    deps =
        [
            ":shared",
        ],
)

cc_test(
    name = "test_mam",
    srcs = [
        "test_mam.c",
    ],
    deps =
        [
            ":shared",
        ],
)

cc_test(
    name = "test",
    srcs = [
        "test.c",
    ],
    deps =
        [
            ":shared",
        ],
)
