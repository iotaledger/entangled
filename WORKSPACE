workspace(name = "org_iota_entangled")

git_repository(
    name = "rules_iota",
    commit = "39eeaf4fad15929b0fcc88b8fc473925a6bd9060",
    remote = "https://github.com/iotaledger/rules_iota.git",
)

git_repository(
    name = "iota_toolchains",
    commit = "6b501df8e7f3bc3b143c894737fbb1d82e914762",
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
