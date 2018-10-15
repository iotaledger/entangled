workspace(name = "org_iota_entangled")

SNAPSHOT_REPOSITORY = "https://raw.githubusercontent.com/thibault-martinez/snapshots/master"

SNAPSHOT_TIMESTAMP_MAINNET = "20180917"

SNAPSHOT_TIMESTAMP_TESTNET = "20180329"

http_file(
    name = "snapshot_conf_mainnet",
    url = SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.json",
)

http_file(
    name = "snapshot_sig_mainnet",
    url = SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.sig",
)

http_file(
    name = "snapshot_mainnet",
    url = SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.txt",
)

http_file(
    name = "snapshot_conf_testnet",
    url = SNAPSHOT_REPOSITORY + "/testnet/" + SNAPSHOT_TIMESTAMP_TESTNET + "/snapshot.json",
)

http_file(
    name = "snapshot_testnet",
    url = SNAPSHOT_REPOSITORY + "/testnet/" + SNAPSHOT_TIMESTAMP_TESTNET + "/snapshot.txt",
)

git_repository(
    name = "rules_iota",
    commit = "8571197145287c75a2b9cd93531db27d765fafc4",
    remote = "https://github.com/iotaledger/rules_iota.git",
)

git_repository(
    name = "iota_toolchains",
    commit = "6b501df8e7f3bc3b143c894737fbb1d82e914762",
    remote = "https://github.com/iotaledger/toolchains.git",
)

new_git_repository(
    name = "iota_lib_cpp",
    build_file = "third_party/iota_lib_cpp/BUILD.bzl",
    commit = "9971c832e6a38972803a4d1506a78c36451c3df3",
    remote = "https://github.com/th0br0/iota.lib.cpp.git",
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
