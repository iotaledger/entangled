cc_library(
    name = "defs",
    hdrs = ["defs.h"],
    deps = [
        "//common/trinary:tryte",
    ],
)

genrule(
    name = "db_file",
    srcs = ["//common/storage/sql:schema"],
    outs = ["ciri.db"],
    cmd = "$(location @sqlite3//:shell) $@ < $<",
    tools = ["@sqlite3//:shell"],
)


cc_test(
    name = "test_get_trytes",
    srcs = ["test_get_trytes.c"],
    data = [":db_file"],
    deps = [
        ":defs",
        "//ciri/api",
        "//consensus/test_utils",
        "@unity",
    ],
)

