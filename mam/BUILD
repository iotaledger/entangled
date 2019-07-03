package(default_visibility = ["//visibility:public"])

load("@iota_toolchains//tools/emscripten:defs.bzl", "emcc_binary")

cc_library(
    name = "defs",
    hdrs = ["defs.h"],
)

# Build command:
# bazel clean (recommended)
# bazel build --spawn_strategy=local --define=workspace=$(bazel info workspace)  --define=output_base=$(bazel info output_base)  --config=emscripten //mam:mam.js

#Uncomment the following rule when you want to build it explicitly

#emcc_binary(
#    name = "mam.js",
#    deps = [
#        "//mam/api",
#    ],
#)

cc_binary(
    name = "libmam.so",
    linkshared = True,
    deps = ["//mam/api"],
)
