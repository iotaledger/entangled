load("@build_bazel_rules_apple//apple:ios.bzl", "ios_static_framework")

ios_static_framework(
    name="ios_bindings",
    bundle_name="EntangledKit",
    hdrs=["Interface.h"],
    deps=[":ios_bindings_classes"],
    minimum_os_version="9.0")

objc_library(
   name="ios_bindings_classes",
   srcs=["Interface.m"],
   hdrs=["Interface.h"],
   deps=["//mobile/interface:interface"])
   

