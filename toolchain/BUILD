package(default_visibility = ["//visibility:public"])

exports_files(["test.sh"])

load(":cc_toolchain_config.bzl", "cc_toolchain_config")

cc_toolchain_suite(
    name = "emscripten",
    toolchains = {
        "emscripten": "emscripten_toolchain",
    },
)

cc_toolchain_config(
    name = "emscripten_toolchain_config",
    cpu = "emscripten",
)

filegroup(name = "empty")

filegroup(
    name = "all",
    srcs = [
        "emar.sh",
        "emcc.sh",
        ":emscripten_cache_content",
        "@emscripten_clang//:all",
        "@emscripten_toolchain//:all",
        "@nodejs//:bin/nodejs/bin/node",
    ],
)

filegroup(
    name = "emscripten_cache_content",
    srcs = glob(["emscripten_cache/**/*"]),
)

cc_toolchain(
    name = "emscripten_toolchain",
    all_files = ":all",
    compiler_files = ":all",
    dwp_files = ":empty",
    linker_files = ":all",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
    toolchain_config = ":emscripten_toolchain_config",
    toolchain_identifier = "emscripten-toolchain",
)
