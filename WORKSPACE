workspace(name="org_iota_entangled")

git_repository(
    name="rules_iota",
    remote="https://gitlab.com/iota-foundation/software/rules_iota.git",
    commit="5622593910361262b248ad165aaf60bc87d0fa16")

android_sdk_repository(
    name="androidsdk",
    api_level=19, )

android_ndk_repository(
    name="androidndk",
    api_level=14, )

load("@rules_iota//:defs.bzl", "iota_deps")
iota_deps()

http_archive(
        name="io_bazel_rules_docker",
        url="https://github.com/bazelbuild/rules_docker/archive/4d8ec6570a5313fb0128e2354f2bc4323685282a.zip",
        strip_prefix=
        "rules_docker-4d8ec6570a5313fb0128e2354f2bc4323685282a",
        sha256=
        "b7b9c74de652fa0a181309218ec72c126d7da09a3f3b270cc0732434617c290e")

load(
    "@io_bazel_rules_docker//container:container.bzl",
    "container_pull",
    container_repositories = "repositories",
)
container_repositories()

container_pull(
  name = "essentials",
  registry = "registry.gitlab.com",
  repository = "iota-foundation/software/entangled/essentials",
  digest = "sha256:25af076537abcb85ef3467712a6cd8e0bc5526e06d3e86e5280d193d44c7b4cf",
)

#cc_image setup
load(
    "@io_bazel_rules_docker//cc:image.bzl",
    _cc_image_repos = "repositories",
)

_cc_image_repos()
