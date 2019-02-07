cc_test(
    name = "test_ntru",
    srcs = ["test_ntru.c"],
    deps = [
        "//mam/v2/ntru",
        "//mam/v2/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_ntru_types",
    srcs = ["test_ntru_types.c"],
    deps = [
        "//mam/v2/ntru:mam_ntru_pk_t_set",
        "//mam/v2/ntru:mam_ntru_sk_t_set",
        "//mam/v2/ntru:ntru_types",
        "@unity",
    ],
)

cc_test(
    name = "test_poly_mred_binary",
    srcs = ["test_poly.c"],
    deps = [
        "//mam/v2/ntru:poly_mred_binary",
        "@unity",
    ],
)

cc_test(
    name = "test_poly_no_mred_binary",
    srcs = ["test_poly.c"],
    deps = [
        "//mam/v2/ntru:poly_no_mred_binary",
        "@unity",
    ],
)
