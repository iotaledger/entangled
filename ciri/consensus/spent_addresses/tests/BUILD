filegroup(
    name = "spent_addresses_test_file",
    srcs = ["spent_addresses_test.txt"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "test_spent_addresses_provider",
    srcs = ["test_spent_addresses_provider.c"],
    data = [
        ":spent_addresses_test_file",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/spent_addresses:spent_addresses_provider",
        "//ciri/consensus/test_utils",
        "//utils:files",
        "@unity",
    ],
)

cc_test(
    name = "test_spent_addresses_service",
    srcs = ["test_spent_addresses_service.c"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/spent_addresses:spent_addresses_service",
        "//ciri/consensus/test_utils",
        "//utils:files",
        "@unity",
    ],
)
