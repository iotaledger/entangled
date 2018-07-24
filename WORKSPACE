workspace(name = "org_iota_entangled")

git_repository(
    name = "rules_iota",
    commit = "16c93b8a3e1669f4f1f155e379536a62f7a41de5",
    remote = "https://github.com/iotaledger/rules_iota.git",
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
