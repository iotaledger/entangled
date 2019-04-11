load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_file")

SNAPSHOT_REPOSITORY = "https://raw.githubusercontent.com/iotaledger/snapshots/master"
SNAPSHOT_TIMESTAMP_MAINNET = "20181222"
SNAPSHOT_TIMESTAMP_TESTNET = "20180329"

def fetch_snapshot_files():
    http_file(
        name = "snapshot_conf_mainnet",
        urls = [SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.json"],
        sha256 = "3aa78a78c91302929778aa8e5b82f153b75c3f9cc8be45f413f3a99b37e70e24",
    )
    http_file(
        name = "snapshot_sig_mainnet",
        urls = [SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.sig"],
        sha256 = "603f34ea67b03a058bc33c216d0778743072f398afb42e63604ce636b1465cd2",
    )
    http_file(
        name = "snapshot_mainnet",
        urls = [SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.txt"],
        sha256 = "f1879f2dc823064aeff256e7777d614d340fd7280d7a1421739bc3d394c2ed8b",
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
