filegroup(
    name = "previous_epochs_spent_addresses_files",
    srcs = [
        "@previous_epochs_spent_addresses_1//file",
        "@previous_epochs_spent_addresses_2//file",
        "@previous_epochs_spent_addresses_3//file",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "spent_addresses_provider",
    srcs = ["spent_addresses_provider.c"],
    hdrs = ["spent_addresses_provider.h"],
    data = [":previous_epochs_spent_addresses_files"],
    visibility = ["//visibility:public"],
    deps = [
        "//common:errors",
        "//common/storage/sql/sqlite3:sqlite3_storage",
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
        "//common:errors",
        "//consensus:conf",
        "//utils:logger_helper",
    ],
)
