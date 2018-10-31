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
`--help` | `-h` | Displays the usage |
`--log-level` | `-l` | Log level amongst: "debug", "info", "notice", "warning", "error", "critical", "alert" and "emergency" | `-l debug`
`--neighbors` | `-n` | Neighbors that you are connected with will be added via this option | `-n "udp://148.148.148.148:14265 udp://[2001:db8:a0b:12f0::1]:14265"`
`--udp-receiver-port` | `-u` | UDP receiver port | `-u 14600`
`--tcp-receiver-port` | `-t` | TCP receiver port | `-t 15600`
`--testnet` | | Makes it possible to run IRI with the IOTA testnet | `--testnet`
