workspace(name = "org_iota_entangled")

git_repository(
    name = "rules_iota",
    commit = "aa09cfc6e6613a25a7b48dcf4ddccbe86b5654c1",
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
