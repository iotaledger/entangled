load("@build_bazel_rules_apple//apple:ios.bzl", "ios_static_framework")

ios_static_framework(
    name = "ios_bindings",
    hdrs = ["Interface.h"],
    bundle_name = "EntangledKit",
    minimum_os_version = "9.0",
    deps = ["//common/helpers"],
)
