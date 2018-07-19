cc_library(
    name = "storage",
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/trinary:trit_array",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = ["//common/storage/sql/sqlite3:sqlite3_storage"],
)
