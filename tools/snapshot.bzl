SNAPSHOT_REPOSITORY = "https://raw.githubusercontent.com/iotaledger/snapshots/master"
SNAPSHOT_TIMESTAMP_MAINNET = "20180917"
SNAPSHOT_TIMESTAMP_TESTNET = "20180329"

def fetch_snapshot_files():
    native.http_file(
        name = "snapshot_conf_mainnet",
        url = SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.json",
        sha256 = "e798f46384cbc747a7192925c446a17097c9aaf304751a6209ee1bcd49c2ea82",
    )
    native.http_file(
        name = "snapshot_sig_mainnet",
        url = SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.sig",
        sha256 = "5257c360d627dcec4c518aa26c58e67d9b93ad58d1797f2747dc3db70abe26a5",
    )
    native.http_file(
        name = "snapshot_mainnet",
        url = SNAPSHOT_REPOSITORY + "/mainnet/" + SNAPSHOT_TIMESTAMP_MAINNET + "/snapshot.txt",
        sha256 = "5c14d76aaea2e864bf578a66571dd1aa7d8d12e611ef122562479e03b8b0c537",
    )
    native.http_file(
        name = "snapshot_conf_testnet",
        url = SNAPSHOT_REPOSITORY + "/testnet/" + SNAPSHOT_TIMESTAMP_TESTNET + "/snapshot.json",
        sha256 = "afec30dfa697852ee12d8c183f86c9d01882c4f9d7d8c4cc58a94488b6d73068",
    )
    native.http_file(
        name = "snapshot_testnet",
        url = SNAPSHOT_REPOSITORY + "/testnet/" + SNAPSHOT_TIMESTAMP_TESTNET + "/snapshot.txt",
        sha256 = "713bbe5e986bbaccae44800aac2f5bdd0f3edc1246e31660a19a8be8265b63b4",
    )
