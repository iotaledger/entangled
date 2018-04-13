load("@build_bazel_rules_apple//apple:ios.bzl", "ios_static_framework")

ios_static_framework(
    name="ios_bindings",
    deps=["//mobile/interface:ios_interface"],
    minimum_os_version="9.0")

