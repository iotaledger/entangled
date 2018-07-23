package(default_visibility = ["//visibility:public"])

cc_library(
    name = "stdint",
    hdrs = ["stdint.h"],
)

cc_library(
    name = "lock_handle",
    hdrs = ["lock_handle.h"],
)

cc_library(
    name = "cond_handle",
    hdrs = ["cond_handle.h"],
    deps = [":lock_handle"],
)

cc_library(
    name = "errors",
    hdrs = ["errors.h"],
)

cc_library(
    name = "logger_helper",
    hdrs = ["logger_helper.h"],
    srcs = ["logger_helper.c"],
    deps = ["@com_github_embear_logger//:logger",
            ":lock_handle"],
)
