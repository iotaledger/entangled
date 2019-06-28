cc_test(
    name = "test_entry_point_selector",
    timeout = "moderate",
    srcs = ["test_entry_point_selector.c"],
    data = [":db_file"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/test_utils",
        "//ciri/consensus/tip_selection/entry_point_selector",
        "//common/helpers:digest",
        "//common/model/tests:defs",
        "//utils:files",
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
