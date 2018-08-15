package(default_visibility = ["//visibility:public"])

config_setting(
    name = "linux",
    constraint_values = [
        "@bazel_tools//platforms:linux",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "cond",
    hdrs = ["cond.h"],
    deps = [":lock"],
)

cc_library(
    name = "lock",
    hdrs = ["lock.h"],
)

cc_library(
    name = "rw_lock",
    hdrs = ["rw_lock.h"],
)

cc_library(
    name = "thread",
    hdrs = ["thread.h"],
    linkopts = select({
        ":linux": ["-pthread"],
        "//conditions:default": [],
    }),
)
