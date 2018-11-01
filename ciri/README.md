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
`--log-level` | `-l` | Log level amongst: "debug", "info", "notice", "warning", "error", "critical", "alert" and "emergency". | `-l debug`
`--mwm`||The minimum weight magnitude is the number of trailing 0s that must appear in the end of a transaction hash. Increasing this number by 1 will result in proof of work that is 3 times as hard.| `--mwm 14`
`--neighbors` | `-n` | URIs of peer iota nodes. | `-n "udp://148.148.148.148:14265 udp://[2001:db8:a0b:12f0::1]:14265"`
`--p-remove-request`| | A number between 0 and 1 that represents the probability of stopping to request a transaction.This number should be closer to 0 so non-existing transaction hashes will eventually be removed. | `--p-remove-request 0.01`
`--p-select-milestone` | | A number between 0 and 1 that represents the probability of requesting a milestone transaction from a neighbor. This should be large since it is imperative that we find milestones to get transactions confirmed. | `--p-select-milestone 0.7`
`--tcp-receiver-port` | `-t` | TCP receiver port. | `-t 15600`
`--udp-receiver-port` | `-u` | UDP receiver port. | `-u 14600`
`--max-get-trytes` | | The maximal number of trytes that may be returned by the "getTrytes" API call. If the number of transactions found exceeds this number an error will be returned. | `--max-get-trytes 10000`
`--port` | `p` | The port that will be used by the API. | `--port 14265`
`--alpha` | | Parameter that defines the randomness of the tip selection. Should be a number between 0 to infinity, where 0 is most random and infinity is most deterministic. | `--alpha 0.001`
`--below-max-depth` | | The maximal number of unconfirmed transactions that may be analyzed in order to find the latest milestone the transaction that we are stepping on during the walk approves. | `--below-max-depth 20000`
`--max-depth` | | The maximal number of previous milestones from where you can perform the random walk. | `--max-depth 15`
