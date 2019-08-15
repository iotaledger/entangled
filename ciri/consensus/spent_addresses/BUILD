cc_library(
    name = "spent_addresses_provider",
    srcs = ["spent_addresses_provider.c"],
    hdrs = ["spent_addresses_provider.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/storage/sql/sqlite3:storage_sqlite3",
        "//utils:logger_helper",
    ],
)

cc_library(
    name = "spent_addresses_service",
    srcs = ["spent_addresses_service.c"],
    hdrs = ["spent_addresses_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":spent_addresses_provider",
        "//ciri/consensus:conf",
        "//ciri/consensus/bundle_validator",
        "//ciri/consensus/tangle",
        "//common:errors",
        "//utils:logger_helper",
    ],
)
