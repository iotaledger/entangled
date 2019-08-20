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
        "//common/storage:spent_addresses_db",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/spent_addresses:spent_addresses_provider",
        "//utils:files",
        "@unity",
    ],
)

cc_test(
    name = "test_spent_addresses_service",
    srcs = ["test_spent_addresses_service.c"],
    data = [
        "//common/storage:spent_addresses_db",
        "//common/storage:tangle_db",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/spent_addresses:spent_addresses_service",
        "//ciri/consensus/test_utils",
        "//utils:files",
        "@unity",
    ],
)
