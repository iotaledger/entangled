cc_test(
    name = "bundle_validator",
    timeout = "moderate",
    srcs = [
        "bundle_validator.c",
    ],
    data = [":db_file"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/bundle_validator",
        "//ciri/consensus/test_utils",
        "//common/storage/tests/helpers",
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
