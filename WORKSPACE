workspace(name = "org_iota_entangled")

git_repository(
    name = "rules_iota",
    commit = "16c93b8a3e1669f4f1f155e379536a62f7a41de5",
    remote = "https://github.com/iotaledger/rules_iota.git",
)

git_repository(
    name = "iota_toolchains",
    commit = "984a2e58bce68c2b74100d9fa1f3434e96e92d95",
    remote = "https://github.com/iotaledger/toolchains.git",
)

git_repository(
    name = "iota_toolchains",
    commit = "a273b913ae18ab80a6e0f5c96e254927ee7c9440",
    remote = "https://github.com/iotaledger/toolchains.git",
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
