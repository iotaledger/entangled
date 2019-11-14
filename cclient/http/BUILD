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
    name = "shared",
    hdrs = ["http.h"],
    visibility = ["//visibility:public"],
    deps = ["//cclient:service"],
)

cc_library(
    name = "http",
    srcs = [
        "http.c",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":shared",
        ":socket",
        "@http_parser",
    ],
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
