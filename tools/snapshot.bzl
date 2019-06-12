load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_file")

SNAPSHOT_REPOSITORY = "https://raw.githubusercontent.com/iotaledger/snapshots/master"
SNAPSHOT_TIMESTAMP_MAINNET = "20190410"
SNAPSHOT_TIMESTAMP_TESTNET = "20180329"

def fetch_snapshot_files():
    http_file(
        name = "snapshot_conf_mainnet",
        urls = [SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.json"],
        sha256 = "f0dbda472b0ed29945ad0d24fd7c5607bc0bd315701377cf6026df926bdcc1db",
    )
    http_file(
        name = "snapshot_sig_mainnet",
        urls = [SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.sig"],
        sha256 = "2b590320faa8581327dd86256327f2d464c5fdbdf4cd42a38da0bcb18b70bf12",
    )
    http_file(
        name = "snapshot_mainnet",
        urls = [SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.txt"],
        sha256 = "f9b1db856bb5ae49ca68ef02513fed21ebc1d6a31e4c5cd541e11a0f2e6a4f05",
    )
    http_file(
        name = "snapshot_conf_testnet",
        urls = [SNAPSHOT_REPOSITORY + "/testnet/" + SNAPSHOT_TIMESTAMP_TESTNET + "/snapshot.json"],
        sha256 = "aab8d01a1698468b03b7031d4f56def00ff927e5b1bccc3efccbda48022fac8e",
    )
    http_file(
        name = "snapshot_testnet",
        urls = [SNAPSHOT_REPOSITORY + "/testnet/" + SNAPSHOT_TIMESTAMP_TESTNET + "/snapshot.txt"],
        sha256 = "713bbe5e986bbaccae44800aac2f5bdd0f3edc1246e31660a19a8be8265b63b4",
    )
