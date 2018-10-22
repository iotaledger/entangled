workspace(name = "org_iota_entangled")

git_repository(
    name = "rules_iota",
    commit = "8571197145287c75a2b9cd93531db27d765fafc4",
    remote = "https://github.com/iotaledger/rules_iota.git",
)

git_repository(
    name = "iota_toolchains",
    commit = "9eeb9c757eabfd1b2ff3235b1ce1f667e29ee07d",
    remote = "https://github.com/iotaledger/toolchains.git",
)

new_git_repository(
    name = "iota_lib_cpp",
    build_file = "third_party/iota_lib_cpp/BUILD.bzl",
    commit = "9971c832e6a38972803a4d1506a78c36451c3df3",
    remote = "https://github.com/th0br0/iota.lib.cpp.git",
)

android_sdk_repository(
    name = "androidsdk",
    api_level = 19,
)

android_ndk_repository(
    name = "androidndk",
    api_level = 14,
)

git_repository(
    name = "build_bazel_rules_apple",
    remote = "https://github.com/bazelbuild/rules_apple.git",
    tag = "0.6.0",
)

git_repository(
    name = "bazel_skylib",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
    tag = "0.5.0",
)

load(
    "@build_bazel_rules_apple//apple:repositories.bzl",
    "apple_rules_dependencies",
)

apple_rules_dependencies()


load("@rules_iota//:defs.bzl", "iota_deps")

iota_deps()

load("@iota_toolchains//:toolchains.bzl", "setup_toolchains")

setup_toolchains()

load("//tools:snapshot.bzl", "fetch_snapshot_files")

fetch_snapshot_files()
