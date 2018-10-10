cc_test(
    name = "test_exit_probability_randomizer",
    srcs = [
        "test_exit_probability_randomizer.c",
    ],
    data = [
        "snapshot.txt",
        ":db_file",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//common/storage/sql/sqlite3:sqlite3_storage",
        "//common/storage/tests/helpers",
        "//common/trinary:trit_ptrit",
        "//consensus/cw_rating_calculator",
        "//consensus/exit_probability_randomizer",
        "//consensus/test_utils",
        "@unity",
    ],
)

genrule(
    name = "db_file",
    srcs = ["//common/storage/sql:schema"],
    outs = ["ciri.db"],
    tools = ["@sqlite3//:shell"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
)
