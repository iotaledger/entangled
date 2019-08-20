cc_library(
    name = "defs",
    hdrs = ["defs.h"],
    deps = [
        "//common/trinary:tryte",
    ],
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
    data = ["//common/storage:tangle_db"],
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
    data = ["//common/storage:tangle_db"],
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
    data = ["//common/storage:tangle_db"],
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
    data = ["//common/storage:tangle_db"],
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
    data = ["//common/storage:tangle_db"],
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
    data = ["//common/storage:tangle_db"],
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
    data = ["//common/storage:tangle_db"],
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
    data = ["//common/storage:tangle_db"],
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
    timeout = "moderate",
    srcs = ["test_were_addresses_spent_from.c"],
    data = [
        "//common/storage:spent_addresses_db",
        "//common/storage:tangle_db",
    ],
    deps = [
        "//ciri/api",
        "//ciri/consensus:conf",
        "//ciri/consensus/test_utils",
        "@unity",
    ],
)
