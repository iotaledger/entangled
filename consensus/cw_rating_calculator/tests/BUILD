cc_test(
    name = "test_cw_ratings_dfs",
    srcs = [
        "test_cw_rating_calculator.c",
    ],
    data = [":db_file"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/storage/sql/sqlite3:sqlite3_storage",
        "//common/storage/tests/helpers",
        "//common/trinary:trit_ptrit",
        "//consensus/cw_rating_calculator",
        "//consensus/test_definitions:defs",
        "@unity",
    ],
)

genrule(
    name = "db_file",
    srcs = ["//common/storage/sql:schema"],
    outs = ["ciri.db"],
    cmd = "sqlite3 $@ < $<",
)
