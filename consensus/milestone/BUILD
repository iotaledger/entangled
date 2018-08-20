cc_library(
    name = "milestone",
    srcs = ["milestone.c"],
    hdrs = ["milestone.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/storage/sql/sqlite3:sqlite3_storage",
        "//common/trinary:trit_array",
        "//consensus/snapshot",
        "//consensus/view_models",
        "//utils:logger_helper",
    ],
)
