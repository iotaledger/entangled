cc_test(
    name = "test_milestone_service",
    srcs = [
        "test_milestone_service.c",
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
        "//consensus/milestone_service",
        "//consensus/milestone_tracker",
        "//consensus/snapshot:snapshots_provider",
        "//consensus/test_utils",
        "//consensus/transaction_solidifier",
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
