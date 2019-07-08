Tracking issues
---------------

- [API](https://github.com/iotaledger/entangled/issues/325)
- [Generic event-routing system](https://github.com/iotaledger/entangled/issues/306)

# cIRI

A low level implementation of an IOTA node in C.

This is a WIP [IOTA](https://iota.org/) node. It allows users to become part of the [IOTA](https://iota.org/) network as both a transaction relay and network information provider through the easy-to-use [API](https://iota.readme.io/reference).

## How to get started

The IOTA network is an independent peer-to-peer network with a first-user, friend-to-friend, network structure:

- As a 'first-user' network, to access the data streams and APIs that other users provide, you must first exchange your IP and port configuration with a current user.

- As a 'friend-to-friend' network, you have the privilege of joining new users into the network through your node by adding them to your approved neighbors list — ensuring that you both broadcast to them and also receive their broadcasts.

You can find neighbors quickly at both our [Discord Community](https://discord.gg/7Gu2mG5) and [Forum](https://forum.iota.org/).

Everyone will be welcoming and very happy to help you get connected. If you want to get tokens for your testcase, please just ask in one of the communication channels.

## Build and run cIRI

cIRI is built and run through [bazel](https://www.bazel.build/).

Clone the project

```
$ git clone https://github.com/iotaledger/entangled.git
$ cd entangled
```

*First build can take some time due to dependencies downloading.*

### For a mainnet node

Create the databases, only the first time:
```
$ sqlite3 ciri/db/tangle-mainnet.db < common/storage/sql/tangle-schema.sql
$ sqlite3 ciri/db/spent-addresses-mainnet.db < common/storage/sql/spent-addresses-schema.sql
```

Build and run cIRI
```
$ bazel run -c opt --define network=mainnet -- ciri # optional flags
```
### For a testnet node

Create the databases, only the first time:
```
$ sqlite3 ciri/db/tangle-testnet.db < common/storage/sql/tangle-schema.sql
$ sqlite3 ciri/db/spent-addresses-testnet.db < common/storage/sql/spent-addresses-schema.sql
```

Build and run cIRI
```
$ bazel run -c opt --define network=testnet -- ciri # optional flags
```



## Configuration

### Configuration file

cIRI configuration file is a [YAML](http://yaml.org/) file with path `ciri/conf.yml`.

An example file `ciri/conf.example.yml` is provided; you can rename it, uncomment and change values of options you want to use.

Example syntax:

```yaml
log-level: debug
neighbors: "tcp://148.148.148.148:14265"
http-port: 14265
```

### Command Line Interface

Long option | Short option | Description | Example input
--- | --- | --- | ---
`--config` | `-c` | Path to the configuration file. | `--config ciri/conf.yml`
`--help` | `-h` | Displays the usage. |
`--log-level` | `-l` | Valid log levels: "debug", "info", "notice", "warning", "error", "critical", "alert" and "emergency". | `-l debug`
`--spent-addresses-db-path` | | Path to the spent addresses database file. | `--spent-addresses-db-path ciri/db/spent-addresses-mainnet.db`
`--tangle-db-path` | | Path to the tangle database file. | `--tangle-db-path ciri/db/tangle-mainnet.db`
`--tangle-db-revalidate` | | Reloads milestones, state of the ledger and transactions metadata from the tangle database. | `--tangle-db-revalidate false`
`--auto-tethering-enabled` | | Whether to accept new connections from unknown neighbors (which are not defined in the config and were not added via addNeighbors). | `--auto-tethering-enabled false`
`--max-neighbors` | | The maximum number of neighbors allowed to be connected. | `--max-neighbors 5`
`--mwm` | | Number of trailing ternary 0s that must appear at the end of a transaction hash. Difficulty can be described as 3^mwm. | `--mwm 14`
`--neighboring-address` | | The address to bind the TCP server socket to. | `--neighboring-address "0.0.0.0"`
`--neighboring-port` | `-t` | The TCP receiver port. | `--neighboring-port 1500`
`--neighbors` | `-n` | URIs of neighbouring nodes, separated by a space. | `-n "tcp://148.148.148.148:14265 tcp://[2001:db8:a0b:12f0::1]:14265"`
`--p-propagate-request` |  | Probability of propagating the request of a transaction to a neighbor node if it can't be found. This should be low since we don't want to propagate non-existing transactions that spam the network. Value must be in [0,1]. | `--p-propagate-request 0.01`
`--p-remove-request` | | Probability of removing a transaction from the request queue without requesting it. Value must be in [0,1]. | `--p-remove-request 0.01`
`--p-reply-random-tip` | | Probability of replying to a random transaction request, even though your node doesn't have anything to request. Value must be in [0,1]. | `--p-reply-random-tip 0.66`
`--p-select-milestone` | | Probability of sending a current milestone request to a neighbour. Value must be in [0,1]. | `--p-select-milestone 0.7`
`--p-send-milestone` | | Probability of sending a milestone transaction when the node looks for a random transaction to send to a neighbor. Value must be in [0,1]. | `--p-send-milestone 0.02`
`--recent-seen-bytes-cache-size` | | The number of entries to keep in the network cache. | `--recent-seen-bytes-cache-size 1500`
`--reconnect-attempt-interval` | | The interval (in seconds) at which to reconnect to neighbors. | `--reconnect-attempt-interval 60`
`--requester-queue-size` | | Size of the transaction requester queue. | `--requester-queue-size 10000`
`--tips-cache-size` | | Size of the tips cache. Also bounds the number of tips returned by getTips API call. | `--tips-cache-size 5000`
`--tips-solidifier-enabled` | | Scan the current tips and attempt to mark them as solid. | `--tips-solidifier-enabled true`
`--http-port` | `-p` | HTTP API listen port. | `--http-port 14265`
`--max-find-transactions` | | The maximal number of transactions that may be returned by the 'findTransactions' API call. If the number of transactions found exceeds this number an error will be returned | `--max-find-transactions 100000`
`--max-get-trytes` | | Maximum number of transactions that will be returned by the 'getTrytes' API call. | `--max-get-trytes 10000`
`--remote-limit-api` | | Commands that should be ignored by API. | `--remote-limit-api "attachToTangle, addNeighbors"`
`--alpha` | | Randomness of the tip selection. Value must be in [0, inf] where 0 is most random and inf is most deterministic. | `--alpha 0.001`
`--below-max-depth` | | Maximum number of unconfirmed transactions that may be analysed to find the latest referenced milestone by the currently visited transaction during the random walk. | `--below-max-depth 20000`
`--coordinator-address` | | The address of the coordinator. | `--coordinator-address "EQS...VD9"`
`--coordinator-depth` | | The depth of the Merkle tree which in turn determines the number of leaves (private keys) that the coordinator can use to sign a message. | `--coordinator-depth 23`
`--coordinator-security-level` | | The security level used in coordinator signatures. | `--coordinator-security-level 2`
`--coordinator-signature-type` | | The signature type used in coordinator signatures. Valid types: "CURL_P27", "CURL_P81" and "KERL". | `--coordinator-signature-type KERL`
`--last-milestone` | | The index of the last milestone issued by the corrdinator before the last snapshot. | `--last-milestone 1050000`
`--max-depth` | | Limits how many milestones behind the current one the random walk can start. | `--max-depth 15`
`--snapshot-file` | | Path to the file that contains the state of the ledger at the last snapshot. | `--snapshot-file external/snapshot_mainnet/file/snapshot.txt`
`--snapshot-signature-depth` | | Depth of the snapshot signature. | `--snapshot-signature-depth 6`
`--snapshot-signature-file` | | Path to the file that contains a signature for the snapshot file. | `--snapshot-signature-file external/snapshot_sig_mainnet/file/snapshot.sig`
`--snapshot-signature-index` | | Index of the snapshot signature. | `--snapshot-signature-index 12`
`--snapshot-signature-pubkey` | | Public key of the snapshot signature. | `--snapshot-signature-pubkey "TTX...YAC"`
`--snapshot-signature-skip-validation` | | Skip validation of snapshot signature. Must be "true" or "false". | `--snapshot-signature-skip-validation false`
`--snapshot-timestamp` | | Epoch time of the last snapshot. | `--snapshot-timestamp 1554904800`
`--spent-addresses-files` | | List of whitespace separated files that contains spent addresses to be merged into the database. | `--spent-addresses-files "file0 file1"`
