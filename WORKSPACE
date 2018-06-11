workspace(name = "org_iota_entangled")

git_repository(
    name = "rules_iota",
    commit = "cf7ffa1d71b3bc751b6cd13166fa34625673fdfd",
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


