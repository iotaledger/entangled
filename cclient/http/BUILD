load(
    "//tools:compiler_flags.bzl",
    "MSVC_WARNING_FLAGS",
    "POSIX_WARNING_FLAGS",
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
    copts = select(
        {
            "@bazel_tools//src/conditions:windows_msvc": MSVC_WARNING_FLAGS,
            "//conditions:default": POSIX_WARNING_FLAGS,
        },
    ),
    visibility = ["//visibility:public"],
    deps = [
        ":shared",
        "//utils/handles:socket",
        "@http_parser",
    ],
)
