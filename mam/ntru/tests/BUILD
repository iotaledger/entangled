cc_test(
    name = "test_ntru",
    timeout = "short",
    srcs = ["test_ntru.c"],
    deps = [
        "//mam/ntru",
        "@unity",
    ],
)

cc_test(
    name = "test_poly_mred_binary",
    timeout = "moderate",
    srcs = ["test_poly.c"],
    deps = [
        "//mam/ntru:poly_mred_binary",
        "@unity",
    ],
)

cc_test(
    name = "test_poly_no_mred_binary",
    timeout = "moderate",
    srcs = ["test_poly.c"],
    deps = [
        "//mam/ntru:poly_no_mred_binary",
        "@unity",
    ],
)
