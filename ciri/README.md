Tracking issues
---------------

- [API](https://github.com/iotaledger/entangled/issues/325)
- [Consensus](https://github.com/iotaledger/entangled/issues/182)
- [Generic event-routing system](https://github.com/iotaledger/entangled/issues/306)

cIRI
====

A low level implementation of an IOTA node in C.

This is a WIP [IOTA](https://iota.org/) node. It allows users to become part of the [IOTA](https://iota.org/) network as both a transaction relay and network information provider through the easy-to-use [API](https://iota.readme.io/reference).

How to get started
------------------

The IOTA network is an independent peer-to-peer network with a first-user, friend-to-friend, network structure:

- As a 'first-user' network, to access the data streams and APIs that other users provide, you must first exchange your IP and port configuration with a current user.

- As a 'friend-to-friend' network, you have the privilege of joining new users into the network through your node by adding them to your approved neighbors list — ensuring that you both broadcast to them and also receive their broadcasts.

You can find neighbors quickly at both our [Discord Community](https://discord.gg/7Gu2mG5) and [Forum](https://forum.iota.org/).

Everyone will be welcoming and very happy to help you get connected. If you want to get tokens for your testcase, please just ask in one of the communication channels.

Build and run cIRI
------------------

cIRI is built and run through [bazel](https://www.bazel.build/).

```
$ git clone https://github.com/iotaledger/entangled.git
$ cd entangled
$ bazel run --define network=mainnet|testnet -- ciri <optional flags>
```

*First build can take some time due to dependencies downloading.*

Command line options
--------------------

*Currently supported, more to come.*

Long option | Short option | Description | Example input
--- | --- | --- | ---
`--help` | `-h` | Displays the usage. |
`--log-level` | `-l` | Valid log levels: "debug", "info", "notice", "warning", "error", "critical", "alert" and "emergency". | `-l debug`
`--db-path` | `-d` | Path to the database file. | `--db-path ciri/db/ciri-mainnet.db`
`--mwm` | | Number of trailing ternary 0s that must appear at the end of a transaction hash. Difficulty can be described as 3^mwm. | `--mwm 14`
`--neighbors` | `-n` | URIs of neighbouring nodes, separated by a space. | `-n "udp://148.148.148.148:14265 udp://[2001:db8:a0b:12f0::1]:14265"`
`--p-propagate-request` |  | Probability of propagating the request of a transaction to a neighbor node if it can't be found. This should be low since we don't want to propagate non-existing transactions that spam the network. Value must be in [0,1]. | `--p-propagate-request 0.01`
`--p-remove-request` | | Probability of removing a transaction from the request queue without requesting it. Value must be in [0,1]. | `--p-remove-request 0.01`
`--p-reply-random-tip` | | Probability of replying to a random transaction request, even though your node doesn't have anything to request. Value must be in [0,1]. | `--p-reply-random-tip 0.66`
`--p-select-milestone` | | Probability of sending a current milestone request to a neighbour. Value must be in [0,1]. | `--p-select-milestone 0.7`
`--p-send-milestone` | | Probability of sending a milestone transaction when the node looks for a random transaction to send to a neighbor. Value must be in [0,1]. | `--p-send-milestone 0.02`
`--tcp-receiver-port` | `-t` | TCP listen port. | `-t 15600`
`--tips-cache-size` | | Size of the tips cache. Also bounds the number of tips returned by getTips API call. | `--tips-cache-size 5000`
`--udp-receiver-port` | `-u` | UDP listen port. | `-u 14600`
`--max-get-trytes` | | Maximum number of transactions that will be returned by the 'getTrytes' API call. | `--max-get-trytes 10000`
`--port` | `p` | HTTP API listen port. | `--port 14265`
`--alpha` | | Randomness of the tip selection. Value must be in [0, inf] where 0 is most random and inf is most deterministic. | `--alpha 0.001`
`--below-max-depth` | | Maximum number of unconfirmed transactions that may be analysed to find the latest referenced milestone by the currently visited transaction during the random walk. | `--below-max-depth 20000`
`--max-depth` | | Limits how many milestones behind the current one the random walk can start. | `--max-depth 15`
