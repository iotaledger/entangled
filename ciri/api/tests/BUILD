cc_library(
    name = "defs",
    hdrs = ["defs.h"],
    deps = [
        "//common/trinary:tryte",
    ],
)

genrule(
    name = "db_file",
    srcs = ["//common/storage/sql:schema"],
    outs = ["ciri.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
)

cc_test(
    name = "test_add_neighbors",
    srcs = ["test_add_neighbors.c"],
    deps = [
        "//ciri/api",
        "@unity",
    ],
)

cc_test(
    name = "test_broadcast_transactions",
    srcs = ["test_broadcast_transactions.c"],
    data = [":db_file"],
    deps = [
        ":defs",
        "//ciri/api",
        "//consensus/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_check_consistency",
    srcs = ["test_check_consistency.c"],
    data = [":db_file"],
    deps = [
        "//ciri/api",
        "//consensus/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_find_transactions",
    srcs = ["test_find_transactions.c"],
    data = [":db_file"],
    deps = [
        ":defs",
        "//ciri/api",
        "//consensus/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_get_node_info",
    srcs = ["test_get_node_info.c"],
    data = [":db_file"],
    deps = [
        "//ciri/api",
        "//consensus/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_get_tips",
    srcs = ["test_get_tips.c"],
    deps = [
        ":defs",
        "//ciri/api",
        "@unity",
    ],
)

cc_test(
    name = "test_get_transactions_to_approve",
    srcs = ["test_get_transactions_to_approve.c"],
    data = [":db_file"],
    deps = [
        "//ciri/api",
        "//consensus/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_get_trytes",
    srcs = ["test_get_trytes.c"],
    data = [":db_file"],
    deps = [
        ":defs",
        "//ciri/api",
        "//consensus/test_utils",
        "@unity",
    ],
)

cc_test(
    name = "test_remove_neighbors",
    srcs = ["test_remove_neighbors.c"],
    deps = [
        "//ciri/api",
        "@unity",
    ],
)

cc_test(
    name = "test_store_transactions",
    srcs = ["test_store_transactions.c"],
    data = [":db_file"],
    deps = [
        ":defs",
        "//ciri/api",
        "//consensus:conf",
        "//consensus/test_utils",
        "@unity",
    ],
)
