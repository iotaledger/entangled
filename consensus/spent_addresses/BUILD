cc_library(
    name = "spent_addresses_provider",
    srcs = ["spent_addresses_provider.c"],
    hdrs = ["spent_addresses_provider.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/storage/sql/sqlite3:sqlite3_storage",
        "//utils:logger_helper",
    ],
)
