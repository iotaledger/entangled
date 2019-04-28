cc_test(
    name = "test_ledger_validator",
    timeout = "moderate",
    srcs = [
        "test_ledger_validator.c",
    ],
    data = [
        ":db_file",
        ":snapshot.txt",
        "//consensus/snapshot/tests:snapshot_test_files",
    ],
    flaky = True,
    visibility = ["//visibility:public"],
    deps = [
        "//common/helpers:digest",
        "//common/storage/sql/sqlite3:sqlite3_storage",
        "//common/storage/tests/helpers",
        "//common/trinary:trit_ptrit",
        "//consensus/ledger_validator",
        "//consensus/milestone:milestone_service",
        "//consensus/milestone:milestone_tracker",
        "//consensus/snapshot:snapshots_provider",
        "//consensus/snapshot:snapshots_service",
        "//consensus/test_utils",
        "//consensus/transaction_solidifier",
        "//utils/containers/hash:hash_uint64_t_map",
        "@unity",
    ],
)

genrule(
    name = "db_file",
    srcs = ["//common/storage/sql:schema"],
    outs = ["ciri.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
)
