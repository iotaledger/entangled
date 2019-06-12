package(default_visibility = ["//visibility:public"])

load("//utils/containers:map_generator.bzl", "map_generate")
load("@iota_toolchains//tools/emscripten:defs.bzl", "emcc_binary")

cc_library(
    name = "api_shared",
    deps = [
        ":trit_t_to_mam_msg_read_context_t_map",
        ":trit_t_to_mam_msg_write_context_t_map",
        "//common:errors",
        "//common/model:bundle",
        "//mam/mam:message",
        "//utils:time",
    ],
)

cc_library(
    name = "api",
    srcs = ["api.c"],
    hdrs = ["api.h"],
    deps = [
        ":api_shared",
    ],
)

map_generate(
    additional_deps = ["//mam/mam:message"],
    additional_include_path = "mam/mam/message.h",
    key_type = "trit_t",
    parent_directory = "mam/api",
    value_type = "mam_msg_write_context_t",
)

map_generate(
    additional_deps = ["//mam/mam:message"],
    additional_include_path = "mam/mam/message.h",
    key_type = "trit_t",
    parent_directory = "mam/api",
    value_type = "mam_msg_read_context_t",
)

# Build command:
# bazel clean (recommended)
# bazel build --spawn_strategy=local --define=workspace=$(bazel info workspace)  --define=output_base=$(bazel info output_base)  --config=emscripten //mam:mam.js

emcc_binary(
    name = "api.js",
    srcs = [
        "api.c",
    ],
    defines = ["MAM_EXPORT_JS"],
    deps = [
        ":api_shared",
    ],
)
