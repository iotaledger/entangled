load(
    "//tools:compiler_flags.bzl",
    "MSVC_DEBUG_FLAGS",
    "MSVC_WARNING_FLAGS",
    "POSIX_DEBUG_FLAGS",
    "POSIX_WARNING_FLAGS",
)

cc_test(
    name = "test_http",
    timeout = "short",
    srcs = [
        "test_http.c",
    ],
    copts = select(
        {
            "@bazel_tools//src/conditions:windows_msvc": MSVC_WARNING_FLAGS + MSVC_DEBUG_FLAGS,
            "//conditions:default": POSIX_WARNING_FLAGS + POSIX_DEBUG_FLAGS,
        },
    ),
    flaky = True,
    deps = [
        "//cclient/http",
        "@cJSON",
        "@unity",
    ],
)
