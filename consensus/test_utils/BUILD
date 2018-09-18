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
    hdrs = ["bundle.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:bundle",
        "//consensus/tangle",
    ],
)
