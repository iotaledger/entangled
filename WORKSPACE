workspace(name = "org_iota_entangled")

git_repository(
    name = "rules_iota",
    commit = "b5e5b91d955f257e47c7803808073658387f8a86",
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


