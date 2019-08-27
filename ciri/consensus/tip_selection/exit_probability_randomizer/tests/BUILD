cc_test(
    name = "test_exit_probability_randomizer",
    timeout = "moderate",
    srcs = ["test_exit_probability_randomizer.c"],
    data = [
        ":snapshot.txt",
        "//ciri/consensus/snapshot/tests:snapshot_test_files",
    ],
    flaky = True,
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/test_utils",
        "//ciri/consensus/tip_selection/cw_rating_calculator",
        "//ciri/consensus/tip_selection/exit_probability_randomizer",
        "//common/storage",
        "//common/storage/tests:defs",
        "//common/trinary:trit_ptrit",
        "@unity",
    ],
)
