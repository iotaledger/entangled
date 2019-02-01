workspace(name = "org_iota_entangled")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

git_repository(
    name = "rules_iota",
    commit = "7a23fc511c5b9707afa412cb25e9b96726038121",
    remote = "https://github.com/iotaledger/rules_iota.git",
)

git_repository(
    name = "iota_toolchains",
    commit = "8df84b381fcc3d87acd605d1053f307276db71df",
    remote = "https://github.com/iotaledger/toolchains.git",
)

git_repository(
    name = "bazel_toolchains",
    commit = "b6875a7bb09b4fa1db8ea347852c0dc9ccae74ab",
    remote = "https://github.com/th0br0/bazel-toolchains.git",
)

new_git_repository(
    name = "iota_lib_cpp",
    build_file = "//:third_party/iota_lib_cpp/BUILD.bzl",
    commit = "9971c832e6a38972803a4d1506a78c36451c3df3",
    remote = "https://github.com/th0br0/iota.lib.cpp.git",
)

android_sdk_repository(
    name = "androidsdk",
    api_level = 19,
)

android_ndk_repository(
    name = "androidndk",
    api_level = 19,
)

load("@rules_iota//:defs.bzl", "iota_deps")
load("@iota_toolchains//:toolchains.bzl", "setup_toolchains")
load("//tools:snapshot.bzl", "fetch_snapshot_files")

iota_deps()

setup_toolchains()

fetch_snapshot_files()
