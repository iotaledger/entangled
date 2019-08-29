package(default_visibility = ["//visibility:public"])

load(
    "//tools:compiler_flags.bzl",
    "MSVC_WARNING_FLAGS",
    "POSIX_WARNING_FLAGS",
)

cc_library(
    name = "defs",
    hdrs = ["defs.h"],
)

cc_library(
    name = "errors",
    srcs = [
        "error_strings.h",
        "errors.c",
        "errors.h",
    ],
    hdrs = ["errors.h"],
    copts = select(
        {
            "@bazel_tools//src/conditions:windows_msvc": MSVC_WARNING_FLAGS,
            "//conditions:default": POSIX_WARNING_FLAGS,
        },
    ),
)

cc_library(
    name = "stdint",
    hdrs = ["stdint.h"],
)
