cc_test(
    name = "bundle_validator",
    srcs = [
        "bundle_validator.c",
    ],
    data = [":db_file"],
    visibility = ["//visibility:public"],
    deps = [
        "//common/storage/tests/helpers",
        "//common/trinary:flex_trit",
        "//consensus:defs",
        "//consensus/bundle_validator",
        "//consensus/test_utils",
        "@unity",
    ],
)

genrule(
    name = "db_file",
    srcs = ["//common/storage/sql:schema"],
    outs = ["ciri.db"],
    cmd = "sqlite3 $@ < $<",
)
