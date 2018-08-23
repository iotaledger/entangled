cc_library(
    name = "view_models",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/model:transaction",
        "//common/storage/sql/sqlite3:sqlite3_storage",
        "//common/trinary:trit_array",
        "//consensus/tangle",
        "//utils:logger_helper",
        "@com_github_uthash//:uthash",
    ],
)
