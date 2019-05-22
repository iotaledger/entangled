# MAM

## What is MAM ?

### Definition

MAM (Masked authenticated messaging) is a trinary messages encryption scheme that allows for both confidentiality and authenticity.

Using MAM2 entities of IOTA can (see Figure 1):
– create channels for broadcasting messages;
– create channel endpoints for protecting messages during broadcasting;
– protect messages in different ways, for example, turn on / off encryption / authentication; – split messages into parts (packets), protect and transmit each part almost independently; – set message recipients and provide them with key material in different ways.

MAM allows for:

- Message encryption via NTRU (public key encrytpion) and PSK (Pre-Shared Key)

- Sending a message to multiple receivers, each having his/her own session key

- Authenticating a message either via MAC (Integrity) or via MSS signature (Integrity + Authenticity)

### Versions

A lot of names have been used in the past, either by the Iota Foundation or by the community, to refer to two iterations of the MAM project. You may have heard, for example: MAM, MAM0, MAM1, MAM2, MAM+... As of now, the name `MAM` will refer to the current fully spec'd and fully revised implementation while `MAM prototype` will refer to the previous implementation.

### Layers

3.7 layer: a set of interconnected algorithms. The algorithms share a common state which is implicitly included in their inputs and outputs (that is, each algorithm can use and modify the common state);

For a more in-depth description of the layers, their states and their algorithms, read [spec.pdf](https://github.com/iotaledger/entangled/blob/develop/mam/spec.pdf).

| Name | Description | Dependencies | Sources |
|------|-------------|---------|
| API | The API layer supports high-level operations that translates MAM messages to/from IOTA bundles. | MAM | [/mam/api](https://github.com/iotaledger/entangled/tree/develop/mam/api) |
| MAM | The MAM layer supports high-level operations of the MAM protocol like writing and reading messages. | MSS, NTRU, PB3, PRNG, PSK | [/mam/mam](https://github.com/iotaledger/entangled/tree/develop/mam/mam) |
| MSS | The MSS layer supports Merkle-tree Signature Scheme. | Sponge, WOTS | [/mam/mss](https://github.com/iotaledger/entangled/tree/develop/mam/mss) |
| NTRU | The NTRU layer supports an NTRU-style public key encryption scheme. | PRNG, Sponge | [/mam/ntru](https://github.com/iotaledger/entangled/tree/develop/mam/ntru) |
| PB3 | The Protobuf3 layer supports encoding, decoding and cryptographic processing of structured data. | Sponge | [/mam/pb3](https://github.com/iotaledger/entangled/tree/develop/mam/pb3) |
| PRNG | The PRNG layer supports the generation of cryptographically strong pseudorandom array of trits. | Sponge | [/mam/prng](https://github.com/iotaledger/entangled/tree/develop/mam/prng) |
| PSK | The PSK layer supports Authenticated Encryption with the means of a Pre-Shared Key. | | [/mam/psk](https://github.com/iotaledger/entangled/tree/develop/mam/psk) |
| Sponge | The sponge layer supports operations based on a sponge function. | Troika | [/mam/sponge](https://github.com/iotaledger/entangled/tree/develop/mam/sponge) |
| Troika | The troika layer supports a trinary hash function used as underlying function of the sponge layer. | | [/mam/troika](https://github.com/iotaledger/entangled/tree/develop/mam/troika) |
| WOTS | The WOTS layer supports Winternitz One-Time Signatures. | PRNG, Sponge | [/mam/wots](https://github.com/iotaledger/entangled/tree/develop/mam/wots) |

## Transport over the Tangle

MAM is a library that read and write IOTA bundles. It must be paired with a client library to actually receive and send them over the Tangle.

3.4 channel: a source of messages. Belongs to an entity. Identified by a public key, called chid, corresponding to which private keys are used to sign either endpoints (main functionality) or messages;

3.5 endpoint: a transmitter of messages. Belongs to a channel. Identified by a public key, called epid, corresponding to which private keys are used to sign messages;
3.6 central endpoint: an endpoint which epid is equal to chid of the corresponding channel;

### Anatomy of a MAM message

## How to use MAM ?

The following is a step by step guide to help you understand and use the core features of MAM.

You can follow it by copying the code snippets step after step. The full example is available [here](https://github.com/iotaledger/entangled/blob/develop/mam/examples/readme.c) and you can try it with the command `bazel run -c opt //mam/examples:readme`.

MAM is a library that allows you to read and write bundles. Sending and receiving bundles to and from the Tangle is out of the scope of MAM so we won't cover that part. To send and receive bundles in C, please refer to [cclient](https://github.com/iotaledger/entangled/tree/develop/cclient).

### Create a seed

To start using MAM, you will need a seed. If you don't know what a seed is or how to generate one, please refer to [what-is-a-seed](https://docs.iota.org/docs/getting-started/0.1/introduction/what-is-a-seed) and/or [create-a-seed](https://docs.iota.org/docs/getting-started/0.1/tutorials/create-a-seed).

With MAM, the following keys will be derived from your seed.
- Session keys
- NTRU private keys
- Pre-Shared Keys
- WOTS private keys

Let's say you generated the seed `MAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLE9`.

> **WARNING**: Do not use your regular token seed for MAM !

> **WARNING**: Do not give access to your seed to anyone !

### Initialize the API

```c
retcode_t mam_api_init(mam_api_t *const api, tryte_t const *const seed);
```

We begin by declaring and initializing an API instance.
```c
mam_api_t api;

mam_api_init(&api, (tryte_t *)"MAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLE9");
```

MAM is a stateful library. The API instance is holding your MAM account information.

- Your channels and their endpoints
- Your own NTRU private keys
- NTRU public keys of your peers
- Pre-Shared Keys
- Contexts of messages you are sending
- Contexts of messages you are receiving

### Create a channel

```c
retcode_t mam_api_create_channel(mam_api_t *const api, size_t const height, tryte_t *const channel_id);
```

We create a new MAM channel by providing the desired height - of the underlying merkle tree - and a tryte array to store the new channel ID. The channel ID is the address where messages created through this channel will be sent to.

Here we choose a height of 5, meaning that we will be able to sign 2^5 = 32 messages with this channel central endpoint.

```c
tryte_t channel_id[MAM_CHANNEL_ID_SIZE];

mam_api_create_channel(&api, 5, channel_id);
```

`channel_id` should be equal to `TZXYUDVIOVZPWIOQRNX9KCYIANVD9MRDINPQBCAQLZAX9JKBWRHHNJQSVVTQRTMXSXSQ9ZTJXDRGHJQGQ`.

### Create an endpoint

```c
retcode_t mam_api_create_endpoint(mam_api_t *const api, size_t const height, tryte_t const *const channel_id, tryte_t *const endpoint_id);
```

We create a new MAM endpoint that belongs to the previously created channel by providing the desired height - of the underlying merkle tree - , the channel ID and a tryte array to store the endpoint ID. Messages created through this endpoint will still be sent to the address of the channel, the channel ID.

Here we choose a height of 5, meaning that we will be able to sign 2^5 = 32 messages with this endpoint.

```c
tryte_t endpoint_id[MAM_ENDPOINT_ID_SIZE];

mam_api_create_endpoint(&api, 5, channel_id, endpoint_id);
```

`endpoint_id` should be equal to `TPRN9JYPHPUOWEOCBLBAGHXYLICDKJSSJYUFNFQVJ9RUKMZATUT9CHOVJCOBDIEYQNKRINODKHARLE9PG`.

### Write a header to a bundle

<!-- TODO NTRU / PSK -->

```c
retcode_t mam_api_bundle_write_header_on_channel(mam_api_t *const api, tryte_t const *const ch_id, mam_psk_t_set_t psks, mam_ntru_pk_t_set_t ntru_pks, bundle_transactions_t *const bundle, trit_t *const msg_id);
retcode_t mam_api_bundle_write_header_on_endpoint(mam_api_t *const api, tryte_t const *const ch_id, tryte_t const *const ep_id, mam_psk_t_set_t psks, mam_ntru_pk_t_set_t ntru_pks, bundle_transactions_t *const bundle, trit_t *const msg_id);
```

Now that we have a channel and an endpoint, we are ready to initiate our first message by writing a header to a bundle. We can do that directly through the central endpoint of the channel, or through the created endpoint. In both cases, the message will be sent to the channel address.

We need to provide the channel ID, the endpoint ID, a set of Pre-Shared Keys, a set of NTRU public keys, a new bundle and a trit array to store the message ID. This message ID uniquely identifies the message inside the channel and will be needed to write packets in the message.

To create the message through the central endpoint
```c
bundle_transactions_t *bundle = NULL;
trit_t message_id[MAM_MSG_ID_SIZE];

bundle_transactions_new(&bundle);
mam_api_bundle_write_header_on_channel(&api, channel_id, NULL, NULL, bundle, message_id);
```

To create the message through the created endpoint (chosen method in the example)
```c
bundle_transactions_t *bundle = NULL;
trit_t message_id[MAM_MSG_ID_SIZE];

bundle_transactions_new(&bundle);
mam_api_bundle_write_header_on_endpoint(&api, channel_id, endpoint_id, NULL, NULL, bundle, message_id);
```

At this point, the bundle can already be sent to the Tangle. You can also add the first packet of the message to this bundle before sending it.

### Write a packet to a bundle

```c
retcode_t mam_api_bundle_write_packet(mam_api_t *const api, trit_t const *const msg_id, tryte_t const *const payload, size_t const payload_size, mam_msg_checksum_t checksum, bool is_last_packet, bundle_transactions_t *const bundle);
```

You can write as many packet as you want in the message. For this, you'll need to provide the message ID, a trytes payload and its size, a checksum method, a boolean value to tell if this is the last packet or an intermediate one and the bundle.

You can choose one of the following.

No checksum
```c
mam_api_bundle_write_packet(&api, message_id, (tryte_t *)"PAYLOAD", 7, MAM_MSG_CHECKSUM_NONE, true, bundle);
```

With MAC checksum
```c
mam_api_bundle_write_packet(&api, message_id, (tryte_t *)"PAYLOAD", 7, MAM_MSG_CHECKSUM_MAC, true, bundle);
```

With signature (chosen method in the example)
```c
mam_api_bundle_write_packet(&api, message_id, (tryte_t *)"PAYLOAD", 7, MAM_MSG_CHECKSUM_SIG, true, bundle);
```

You now have a MAM bundle ready to be sent and received to and from the Tangle.

### Read packets

```c
mam_api_bundle_read(&api, bundle, &payload, &payload_size, &is_last_packet);
```

### Destroy the API

```c
retcode_t mam_api_destroy(mam_api_t *const api);
```

When you are done using MAM, you must destroy the API to release resources it was using.

```c
mam_api_destroy(&api);
```
