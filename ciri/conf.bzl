CIRI_VERSION = ["CIRI_VERSION='\"0.1.0-alpha\"'"]

CIRI_MAINNET_VARIABLES = [
    "TANGLE_DB_PATH='\"ciri/db/tangle-mainnet.db\"'",
    "SPENT_ADDRESSES_DB_PATH='\"ciri/db/spent-addresses-mainnet.db\"'",
    "CIRI_NAME='\"cIRI-mainnet\"'",
] + CIRI_VERSION

CIRI_TESTNET_VARIABLES = [
    "TANGLE_DB_PATH='\"ciri/db/tangle-testnet.db\"'",
    "SPENT_ADDRESSES_DB_PATH='\"ciri/db/spent-addresses-testnet.db\"'",
    "CIRI_NAME='\"cIRI-testnet\"'",
] + CIRI_VERSION
