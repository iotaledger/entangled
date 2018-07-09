package(default_visibility = ["//visibility:public"])

cc_library(
    name = "stdint",
    hdrs = ["stdint.h"],
)

cc_library(
    name = "lock_handle",
    srcs = ["lock_handle.c"],
    hdrs = ["lock_handle.h"],
)

cc_library(
    name = "cond_handle",
    srcs = ["cond_handle.c"],
    hdrs = ["cond_handle.h"],
    deps = [":lock_handle"],
)
