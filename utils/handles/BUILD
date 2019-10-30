package(default_visibility = ["//visibility:public"])

config_setting(
    name = "linux_platform",
    constraint_values = [
        "@bazel_tools//platforms:linux",
    ],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "windows_platform",
    constraint_values = [
        "@bazel_tools//platforms:windows",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "cond",
    hdrs = ["cond.h"],
    linkopts = select({
        ":linux_platform": ["-pthread"],
        "//conditions:default": [],
    }),
    deps = [
        ":lock",
    ] + select({
        ":windows_platform": ["//utils:windows"],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "lock",
    hdrs = ["lock.h"],
    linkopts = select({
        ":linux_platform": ["-pthread"],
        "//conditions:default": [],
    }),
    deps = select({
        ":windows_platform": ["//utils:windows"],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "rw_lock",
    hdrs = ["rw_lock.h"],
    linkopts = select({
        ":linux_platform": ["-pthread"],
        "//conditions:default": [],
    }),
    deps = select({
        ":windows_platform": ["//utils:windows"],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "rand",
    hdrs = ["rand.h"],
)

cc_library(
    name = "thread",
    hdrs = ["thread.h"],
    linkopts = select({
        ":linux_platform": ["-pthread"],
        "//conditions:default": [],
    }),
    deps = select({
        ":windows_platform": ["//utils:windows"],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "signal",
    hdrs = ["signal.h"],
    deps = ["//utils:logger_helper"],
)

cc_library(
    name = "socket",
    srcs = ["socket.c"],
    hdrs = ["socket.h"],
    deps = [
        "//common:errors",
        "@mbedtls",
    ] + select({
        ":windows_platform": ["//utils:windows"],
        "//conditions:default": [],
    }),
)
