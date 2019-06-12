filegroup(
    name = "spent_addresses_test_file",
    srcs = ["spent_addresses_test.txt"],
    visibility = ["//visibility:public"],
)

genrule(
    name = "db_file",
    srcs = ["//common/storage/sql:spent-addresses-schema"],
    outs = ["spent-addresses.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
)

cc_test(
    name = "test_spent_addresses_provider",
    srcs = ["test_spent_addresses_provider.c"],
    data = [
        ":db_file",
        ":spent_addresses_test_file",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//consensus/spent_addresses:spent_addresses_provider",
        "//utils:files",
        "@unity",
    ],
)
