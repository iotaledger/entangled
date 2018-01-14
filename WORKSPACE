workspace(name="org_iota_entangled")

git_repository(
    name="rules_iota",
    remote="https://gitlab.com/iota-foundation/software/rules_iota.git",
    commit="31b4f2e2abadc9b5007401b13c9f79c5bfdb5834")

android_sdk_repository(
    name="androidsdk",
    api_level=19, )

android_ndk_repository(
    name="androidndk",
    api_level=14, )

load("@rules_iota//:defs.bzl", "iota_deps")
iota_deps()
