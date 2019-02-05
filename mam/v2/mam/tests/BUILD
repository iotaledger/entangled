cc_test(
    name = "test_mam",
    srcs = ["test_mam.c"],
    deps = [
        "//mam/v2/mam",
        "//mam/v2/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_mam_types",
    srcs = ["test_mam_types.c"],
    deps = [
        "//mam/v2/mam:mam_pre_shared_key_t_set",
        "//mam/v2/mam:mam_types",
        "@unity",
    ],
)

cc_test(
    name = "test_endpoint",
    srcs = ["test_endpoint.c"],
    deps = [
        "//mam/v2/mam",
        "//mam/v2/mam:endpoint",
        "//mam/v2/mam:mam_endpoint_t_set",
        "//mam/v2/test_utils",
        "@unity",
    ],
)
