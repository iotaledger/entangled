filegroup(
    name = "spent_addresses_test_file",
    srcs = ["spent_addresses_test.txt"],
    visibility = ["//visibility:public"],
)

genrule(
    name = "spent_addresses_db_file",
    srcs = ["//common/storage/sql:spent-addresses-schema"],
    outs = ["spent-addresses.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
)

genrule(
    name = "tangle_db_file",
    srcs = ["//common/storage/sql:tangle-schema"],
    outs = ["tangle.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
)

cc_test(
    name = "test_spent_addresses_provider",
    srcs = ["test_spent_addresses_provider.c"],
    data = [
        ":spent_addresses_db_file",
        ":spent_addresses_test_file",
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
        ":spent_addresses_db_file",
        ":tangle_db_file",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/spent_addresses:spent_addresses_service",
        "//ciri/consensus/test_utils",
        "//utils:files",
        "@unity",
    ],
)
