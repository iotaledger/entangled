package(default_visibility = ["//visibility:public"])

load("@iota_toolchains//tools/emscripten:defs.bzl", "emcc_binary")

cc_library(
    name = "defs",
    hdrs = ["defs.h"],
)

# Build command:
# bazel clean (recommended)
# bazel build --spawn_strategy=local --define=workspace=$(bazel info workspace)  --define=output_base=$(bazel info output_base)  --config=emscripten //mam:mam.js

emcc_binary(
    name = "mam.js",
    linkopts = ["-s LINKABLE=1 -s EXPORT_ALL=1"],
    deps = ["//mam/api"],
)
