cc_test(
    name = "test_ptrit",
    srcs = ["test_trit_ptrit.c"],
    deps = [
        "//common/trinary:trit_ptrit",
        "@unity",
    ],
)

cc_test(
    name = "test_byte",
    srcs = ["test_trit_byte.c"],
    deps = [
        "//common/trinary:trit_byte",
        "@unity",
    ],
)

cc_test(
    name = "test_tryte",
    srcs = ["test_trit_tryte.c"],
    deps = [
        "//common/trinary:trit_tryte",
        "@unity",
    ],
)

cc_test(
    name = "test_long",
    srcs = ["test_trit_long.c"],
    deps = [
        "//common/trinary:trit_long",
        "@unity",
    ],
)

cc_test(
    name = "test_tryte_long",
    srcs = ["test_tryte_long.c"],
    deps = [
        "//common/trinary:tryte_long",
        "@unity",
    ],
)

cc_test(
    name = "test_add",
    srcs = ["test_add.c"],
    deps = [
        "//common/trinary:add",
        "@unity",
    ],
)

cc_test(
    name = "test_trit_array",
    srcs = ["test_trit_array.c"],
    deps = [
        "//common/trinary:trit_array",
        "//common/trinary:trit_byte",
        "//common/trinary:trit_tryte",
        "@unity",
    ],
)

cc_test(
    name = "test_trit_array_3",
    srcs = ["test_trit_array.c"],
    defines = ["TRIT_ARRAY_ENCODING_3_TRITS_PER_BYTE"],
    deps = [
        "//common/trinary:trit_array_3",
        "//common/trinary:trit_byte",
        "//common/trinary:trit_tryte",
        "@unity",
    ],
)

cc_test(
    name = "test_trit_array_4",
    srcs = ["test_trit_array.c"],
    defines = ["TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE"],
    deps = [
        "//common/trinary:trit_array_4",
        "//common/trinary:trit_byte",
        "//common/trinary:trit_tryte",
        "@unity",
    ],
)

cc_test(
    name = "test_trit_array_5",
    srcs = ["test_trit_array.c"],
    defines = ["TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE"],
    deps = [
        "//common/trinary:trit_array_5",
        "//common/trinary:trit_byte",
        "//common/trinary:trit_tryte",
        "@unity",
    ],
)

cc_test(
    name = "test_flex_trit_array",
    srcs = ["test_flex_trit_array.cc"],
    deps = [
        "//common/trinary:flex_trit_array",
        "//common/trinary:trit_array",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "test_flex_trit_array_3",
    srcs = ["test_flex_trit_array.cc"],
    defines = ["TRIT_ARRAY_ENCODING_3_TRITS_PER_BYTE"],
    deps = [
        "//common/trinary:flex_trit_array_3",
        "//common/trinary:trit_array_3",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "test_flex_trit_array_4",
    srcs = ["test_flex_trit_array.cc"],
    defines = ["TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE"],
    deps = [
        "//common/trinary:flex_trit_array_4",
        "//common/trinary:trit_array_4",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "test_flex_trit_array_5",
    srcs = ["test_flex_trit_array.cc"],
    defines = ["TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE"],
    deps = [
        "//common/trinary:flex_trit_array_5",
        "//common/trinary:trit_array_5",
        "@com_google_googletest//:gtest_main",
    ],
)
