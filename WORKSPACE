workspace(name = "org_iota_entangled")

git_repository(
    name = "rules_iota",
    commit = "893bc942f22aa6ad5bbd72e7d86c94452fbd76d8",
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

load("@rules_iota//:defs.bzl", "iota_deps")

iota_deps()

load("@iota_toolchains//:toolchains.bzl", "setup_toolchains")

setup_toolchains()

load("//tools:snapshot.bzl", "fetch_snapshot_files")

fetch_snapshot_files()
