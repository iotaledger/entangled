workspace(name = "org_iota_entangled")

git_repository(
    name = "rules_iota",
    commit = "cf7ffa1d71b3bc751b6cd13166fa34625673fdfd",
    remote = "https://gitlab.com/iota-foundation/software/rules_iota.git",
)

android_sdk_repository(
    name = "androidsdk",
    api_level = 19,
)

android_ndk_repository(
    name = "androidndk",
    api_level = 14,
)

git_repository(
    name = "build_bazel_rules_apple",
    remote = "https://github.com/bazelbuild/rules_apple.git",
    tag = "0.4.0",
)
git_repository(
    name = "bazel_skylib",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
    tag = "0.3.1",
)


load("@rules_iota//:defs.bzl", "iota_deps")

iota_deps()


