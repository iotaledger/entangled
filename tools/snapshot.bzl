load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_file")

SNAPSHOT_TIMESTAMP_MAINNET = "20190410"
SNAPSHOT_TIMESTAMP_TESTNET = "20180329"

SNAPSHOT_FILES_URL = "https://raw.githubusercontent.com/thibault-martinez/snapshots/blob/20190410-previous-epochs-spent-addresses"

def fetch_snapshot_files():
    http_file(
        name = "snapshot_conf_mainnet",
        urls = [SNAPSHOT_FILES_URL + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.json"],
        sha256 = "f0dbda472b0ed29945ad0d24fd7c5607bc0bd315701377cf6026df926bdcc1db",
    )
    http_file(
        name = "snapshot_sig_mainnet",
        urls = [SNAPSHOT_FILES_URL + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.sig"],
        sha256 = "2b590320faa8581327dd86256327f2d464c5fdbdf4cd42a38da0bcb18b70bf12",
    )
    http_file(
        name = "snapshot_mainnet",
        urls = [SNAPSHOT_FILES_URL + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.txt"],
        sha256 = "f9b1db856bb5ae49ca68ef02513fed21ebc1d6a31e4c5cd541e11a0f2e6a4f05",
    )
    http_file(
        name = "snapshot_conf_testnet",
        urls = [SNAPSHOT_FILES_URL + "/testnet/" + SNAPSHOT_TIMESTAMP_TESTNET + "/snapshot.json"],
        sha256 = "aab8d01a1698468b03b7031d4f56def00ff927e5b1bccc3efccbda48022fac8e",
    )
    http_file(
        name = "snapshot_testnet",
        urls = [SNAPSHOT_FILES_URL + "/testnet/" + SNAPSHOT_TIMESTAMP_TESTNET + "/snapshot.txt"],
        sha256 = "713bbe5e986bbaccae44800aac2f5bdd0f3edc1246e31660a19a8be8265b63b4",
    )

SPENT_ADDRESSES_FILES_URL = "https://media.githubusercontent.com/media/thibault-martinez/snapshots/20190410-previous-epochs-spent-addresses"

def fetch_previous_epochs_spent_addresses_files():
    http_file(
        name = "previous_epochs_spent_addresses_1",
        urls = [SPENT_ADDRESSES_FILES_URL + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/previousEpochsSpentAddresses1.txt"],
        sha256 = "3e9eccb30a7baaf9204f8cb6ed4b211f02d18e2bc263cb25df87b9c5fd42db4b",
    )
    http_file(
        name = "previous_epochs_spent_addresses_2",
        urls = [SPENT_ADDRESSES_FILES_URL + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/previousEpochsSpentAddresses2.txt"],
        sha256 = "9d107d8e0f62a12642a58c80b7ff4d1373abf69a76b89a0371d426c2630c4b88",
    )
    http_file(
        name = "previous_epochs_spent_addresses_3",
        urls = [SPENT_ADDRESSES_FILES_URL + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/previousEpochsSpentAddresses3.txt"],
        sha256 = "36f1f46c8b0ef88b89808962886a83e8b44b3ac8a4a7e7da29b7fbaaaa2a7c5f",
    )
