load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_file")

SNAPSHOT_REPOSITORY = "https://raw.githubusercontent.com/iotaledger/snapshots/master"
SNAPSHOT_TIMESTAMP_MAINNET = "20181222"
SNAPSHOT_TIMESTAMP_TESTNET = "20180329"

def fetch_snapshot_files():
    http_file(
        name = "snapshot_conf_mainnet",
        urls = [SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.json"],
        sha256 = "e51ca0324704d79655962b63c90f76bfbae12f35f72124d7608b57fdeafd5caf",
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
        sha256 = "afec30dfa697852ee12d8c183f86c9d01882c4f9d7d8c4cc58a94488b6d73068",
    )
    http_file(
        name = "snapshot_testnet",
        urls = [SNAPSHOT_REPOSITORY + "/testnet/" + SNAPSHOT_TIMESTAMP_TESTNET + "/snapshot.txt"],
        sha256 = "713bbe5e986bbaccae44800aac2f5bdd0f3edc1246e31660a19a8be8265b63b4",
    )
