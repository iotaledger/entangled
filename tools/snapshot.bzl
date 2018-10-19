SNAPSHOT_REPOSITORY = "https://raw.githubusercontent.com/thibault-martinez/snapshots/master"
SNAPSHOT_TIMESTAMP_MAINNET = "20180917"
SNAPSHOT_TIMESTAMP_TESTNET = "20180329"

def fetch_snapshot_files():
    native.http_file(
        name = "snapshot_conf_mainnet",
        url = SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.json",
    )
    native.http_file(
        name = "snapshot_sig_mainnet",
        url = SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.sig",
    )
    native.http_file(
        name = "snapshot_mainnet",
        url = SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.txt",
    )
    native.http_file(
        name = "snapshot_conf_testnet",
        url = SNAPSHOT_REPOSITORY + "/testnet/" + SNAPSHOT_TIMESTAMP_TESTNET + "/snapshot.json",
    )
    native.http_file(
        name = "snapshot_testnet",
        url = SNAPSHOT_REPOSITORY + "/testnet/" + SNAPSHOT_TIMESTAMP_TESTNET + "/snapshot.txt",
    )
