workspace(name = "org_iota_entangled")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "rules_iota",
    commit = "73f598ad1ce3ba79ff22d747f723d6d5cbf351e1",
    remote = "https://github.com/iotaledger/rules_iota.git",
)

git_repository(
    name = "iota_toolchains",
    commit = "0f93f76fdff9f091dd3a99aa3f19b5d3f4f7f7fc",
    remote = "https://github.com/iotaledger/toolchains.git",
)

git_repository(
    name = "bazel_toolchains",
    commit = "b6875a7bb09b4fa1db8ea347852c0dc9ccae74ab",
    remote = "https://github.com/th0br0/bazel-toolchains.git",
)

android_sdk_repository(
    name = "androidsdk",
    api_level = 19,
)

android_ndk_repository(
    name = "androidndk",
    api_level = 19,
)

git_repository(
    name = "build_bazel_rules_apple",
    remote = "https://github.com/bazelbuild/rules_apple.git",
    tag = "0.17.2",
)

load(
    "@build_bazel_rules_apple//apple:repositories.bzl",
    "apple_rules_dependencies",
)

apple_rules_dependencies()

git_repository(
    name = "io_bazel_rules_docker",
    remote = "https://github.com/bazelbuild/rules_docker.git",
    tag = "v0.8.1",
)

load(
    "@io_bazel_rules_docker//repositories:repositories.bzl",
    container_repositories = "repositories",
)

container_repositories()

load(
    "@io_bazel_rules_docker//cc:image.bzl",
    _cc_image_repos = "repositories",
)

_cc_image_repos()

load("@rules_iota//:defs.bzl", "iota_deps")
load("//tools:snapshot.bzl", "fetch_snapshot_files")

iota_deps()

fetch_snapshot_files()

load("@iota_toolchains//:toolchains.bzl", "setup_initial_deps")

setup_initial_deps()

load("@iota_toolchains//:defs.bzl", "setup_toolchains_repositories")

setup_toolchains_repositories()
