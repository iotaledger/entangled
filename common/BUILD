package(default_visibility = ["//visibility:public"])

cc_library(
    name = "stdint",
    hdrs = ["stdint.h"],
)

cc_library(
    name = "cond_handle",
    hdrs = ["cond_handle.h"],
    deps = [":lock_handle"],
)

cc_library(
    name = "lock_handle",
    hdrs = ["lock_handle.h"],
)

cc_library(
    name = "rw_lock_handle",
    hdrs = ["rw_lock_handle.h"],
)

cc_library(
    name = "thread_handle",
    hdrs = ["thread_handle.h"],
    linkopts = ["-lpthread"],
)

cc_library(
    name = "errors",
    hdrs = ["errors.h"],
)

cc_library(
    name = "logger_helper",
    srcs = ["logger_helper.c"],
    hdrs = ["logger_helper.h"],
    deps = [
        ":lock_handle",
        "@com_github_embear_logger//:logger",
    ],
)
