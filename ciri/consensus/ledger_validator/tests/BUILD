cc_test(
    name = "test_ledger_validator",
    timeout = "long",
    srcs = [
        "test_ledger_validator.c",
    ],
    data = [
        ":db_file",
        ":snapshot.txt",
        "//ciri/consensus/snapshot/tests:snapshot_test_files",
    ],
    flaky = True,
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/ledger_validator",
        "//ciri/consensus/milestone:milestone_service",
        "//ciri/consensus/milestone:milestone_tracker",
        "//ciri/consensus/snapshot:snapshots_provider",
        "//ciri/consensus/snapshot:snapshots_service",
        "//ciri/consensus/test_utils",
        "//ciri/consensus/transaction_solidifier",
        "//common/helpers:digest",
        "//common/storage/sql/sqlite3:storage_sqlite3",
        "//common/storage/tests/helpers",
        "//common/trinary:trit_ptrit",
        "//utils/containers/hash:hash_uint64_t_map",
        "@unity",
    ],
)

genrule(
    name = "db_file",
    srcs = ["//common/storage/sql:tangle-schema"],
    outs = ["ciri.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
)
