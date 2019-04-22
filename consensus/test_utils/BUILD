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
        "//common:errors",
        "//common/storage/sql/sqlite3:sqlite3_storage",
        "//consensus/tangle",
        "//utils:files",
    ],
)

cc_library(
    name = "bundle",
    srcs = ["bundle.c"],
    hdrs = ["bundle.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/helpers:digest",
        "//common/helpers:pow",
        "//common/helpers:sign",
        "//common/model:bundle",
        "//consensus/tangle",
        "//utils:macros",
        "//utils:time",
    ],
)
