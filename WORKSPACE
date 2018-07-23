workspace(name = "org_iota_entangled")

git_repository(
    name = "rules_iota",
    commit = "ff1893ef5f17f2c5bef2b1750609adb6b44e68a9",
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
