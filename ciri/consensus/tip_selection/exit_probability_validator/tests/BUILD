cc_test(
    name = "test_exit_probability_validator",
    timeout = "moderate",
    srcs = ["exit_probability_validator.c"],
    data = [
        ":snapshot.txt",
        "//ciri/consensus/snapshot/tests:snapshot_test_files",
        "//common/storage:tangle_db",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/test_utils",
        "//ciri/consensus/tip_selection/exit_probability_validator",
        "//common/storage/tests:defs",
        "//common/trinary:trit_ptrit",
        "@unity",
    ],
)
