cc_test(
    name = "test_entry_point_selector",
    timeout = "moderate",
    srcs = ["test_entry_point_selector.c"],
    data = ["//common/storage:tangle_db"],
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
