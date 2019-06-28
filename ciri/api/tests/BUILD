cc_library(
    name = "defs",
    hdrs = ["defs.h"],
    deps = [
        "//common/trinary:tryte",
    ],
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
    name = "test_add_neighbors",
    timeout = "short",
    srcs = ["test_add_neighbors.c"],
    deps = [
        "//ciri/api",
        "@unity",
    ],
)

cc_test(
    name = "test_attach_to_tangle",
    timeout = "short",
    srcs = ["test_attach_to_tangle.c"],
    deps = [
        "//ciri/api",
        "//ciri/consensus/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_broadcast_transactions",
    timeout = "moderate",
    srcs = ["test_broadcast_transactions.c"],
    data = [":tangle_db_file"],
    deps = [
        ":defs",
        "//ciri/api",
        "//ciri/consensus/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_check_consistency",
    timeout = "moderate",
    srcs = ["test_check_consistency.c"],
    data = [":tangle_db_file"],
    deps = [
        "//ciri/api",
        "//ciri/consensus/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_find_transactions",
    timeout = "moderate",
    srcs = ["test_find_transactions.c"],
    data = [":tangle_db_file"],
    deps = [
        ":defs",
        "//ciri/api",
        "//ciri/consensus/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_get_missing_transactions",
    timeout = "moderate",
    srcs = ["test_get_missing_transactions.c"],
    data = [":tangle_db_file"],
    deps = [
        "//ciri/api",
        "//ciri/consensus/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_get_neighbors",
    timeout = "short",
    srcs = ["test_get_neighbors.c"],
    deps = [
        "//ciri/api",
        "@unity",
    ],
)

cc_test(
    name = "test_get_node_info",
    timeout = "moderate",
    srcs = ["test_get_node_info.c"],
    data = [":tangle_db_file"],
    deps = [
        "//ciri/api",
        "//ciri/consensus/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_get_tips",
    timeout = "short",
    srcs = ["test_get_tips.c"],
    deps = [
        "//ciri/api",
        "@unity",
    ],
)

cc_test(
    name = "test_get_transactions_to_approve",
    timeout = "moderate",
    srcs = ["test_get_transactions_to_approve.c"],
    data = [":tangle_db_file"],
    deps = [
        "//ciri/api",
        "//ciri/consensus/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_get_trytes",
    timeout = "short",
    srcs = ["test_get_trytes.c"],
    data = [":tangle_db_file"],
    deps = [
        ":defs",
        "//ciri/api",
        "//ciri/consensus/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_remove_neighbors",
    timeout = "short",
    srcs = ["test_remove_neighbors.c"],
    deps = [
        "//ciri/api",
        "@unity",
    ],
)

cc_test(
    name = "test_store_transactions",
    timeout = "moderate",
    srcs = ["test_store_transactions.c"],
    data = [":tangle_db_file"],
    deps = [
        ":defs",
        "//ciri/api",
        "//ciri/consensus:conf",
        "//ciri/consensus/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_were_addresses_spent_from",
    timeout = "short",
    srcs = ["test_were_addresses_spent_from.c"],
    data = [
        ":spent_addresses_db_file",
        ":tangle_db_file",
    ],
    deps = [
        "//ciri/api",
        "//ciri/consensus:conf",
        "//ciri/consensus/test_utils",
        "@unity",
    ],
)
