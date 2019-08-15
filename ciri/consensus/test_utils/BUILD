cc_library(
    name = "test_utils",
    visibility = ["//visibility:public"],
    deps = [
        ":bundle",
        ":tangle_setup",
    ],
)

cc_library(
    name = "tangle_setup",
    srcs = ["tangle.c"],
    hdrs = ["tangle.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/tangle",
        "//common:errors",
        "//common/storage/sql/sqlite3:storage_sqlite3",
        "//utils:files",
    ],
)

cc_library(
    name = "bundle",
    srcs = ["bundle.c"],
    hdrs = ["bundle.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ciri/consensus/tangle",
        "//common:errors",
        "//common/helpers:digest",
        "//common/helpers:pow",
        "//common/helpers:sign",
        "//common/model:bundle",
        "//utils:macros",
        "//utils:time",
    ],
)
