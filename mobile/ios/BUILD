load("@build_bazel_rules_apple//apple:ios.bzl", "ios_static_framework")

ios_static_framework(
    name = "ios_bindings",
    hdrs = ["Interface.h"],
    bundle_name = "EntangledKit",
    minimum_os_version = "9.0",
    deps = [":ios_bindings_classes"],
)

objc_library(
    name = "ios_bindings_classes",
    srcs = ["Interface.mm"],
    hdrs = ["Interface.h"],
    deps = [
        "//common/helpers",
        "//utils:bundle_miner",
    ],
)
