cc_test(
    name = "test_exit_probability_randomizer",
    timeout = "moderate",
    srcs = ["test_exit_probability_randomizer.c"],
    data = [
        ":snapshot.txt",
        "//ciri/consensus/snapshot/tests:snapshot_test_files",
        "//common/storage:tangle_db",
    ],
    flaky = True,
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/test_utils",
        "//ciri/consensus/tip_selection/cw_rating_calculator",
        "//ciri/consensus/tip_selection/exit_probability_randomizer",
        "//common/storage/sql/sqlite3:storage_sqlite3",
        "//common/storage/tests/helpers",
        "//common/trinary:trit_ptrit",
        "@unity",
    ],
)
