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
    name = "test_trit_array_1",
    srcs = ["test_trit_array.c"],
    defines = ["TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE"],
    deps = [
        "//common/trinary:trit_array_1",
        "//common/trinary:trit_byte",
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
        "@unity",
    ],
)
