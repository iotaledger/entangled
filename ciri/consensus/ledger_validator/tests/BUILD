cc_test(
    name = "test_ledger_validator",
    timeout = "long",
    srcs = [
        "test_ledger_validator.c",
    ],
    data = [
        ":snapshot.txt",
        "//ciri/consensus/snapshot/tests:snapshot_test_files",
        "//common/storage:tangle_db",
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
        "//common/storage",
        "//common/storage/tests:defs",
        "//common/trinary:trit_ptrit",
        "//utils/containers/hash:hash_uint64_t_map",
        "@unity",
    ],
)
