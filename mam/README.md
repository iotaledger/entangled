# MAM

## Contents

- [Glossary](#glossary)
- [What is MAM ?](#what-is-mam-)
  - [Definition](#definition)
  - [Versions](#versions)
  - [Layers](#layers)
- [Transport over the Tangle](#transport-over-the-Tangle)
  - [Anatomy of a MAM message](#anatomy-of-a-mam-message)
- [How to use MAM API ?](#how-to-use-mam-api-)
  - [Create a seed](#create-a-seed)
  - [Initialize the API](#initialize-the-api)
  - [Create a channel](#create-a-channel)
  - [Create an endpoint](#create-an-endpoint)
  - [Write a header to a bundle](#write-a-header-to-a-bundle)
  - [Write a packet to a bundle](#write-a-packet-to-a-bundle)
  - [Read a packet from a bundle](#read-a-packet-from-a-bundle)
  - [Save and load the API](#save-and-load-the-api)
  - [Destroy the API](#destroy-the-api)
- [Security considerations](#security-considerations)

## Glossary

- Seed: [what-is-a-seed](https://docs.iota.org/docs/getting-started/0.1/introduction/what-is-a-seed) and/or [create-a-seed](https://docs.iota.org/docs/getting-started/0.1/tutorials/create-a-seed).
- Channel
- Endpoint
- PSKs
- Public key (it should be noted that there are different PKs: NTRUpk, chid, epid, IOTA address)
- Integrity
- Privacy
- Authenticity
- AE
- Hash
- Sponge
- NTRU
- MSS
- WOTS
- Message
- Packet

## What is MAM ?

### Definition

Masked Authenticated Messaging (MAM) is a second layer data communication protocol which adds functionality to emit and access encrypted data streams, like RSS, over the Tangle. IOTA’s consensus protocol adds integrity to these message streams. Given these properties, MAM fulfills an important need in industries where integrity and privacy meet.

Using MAM you can:

- create channels for broadcasting messages;
- create channel endpoints for protecting messages during broadcasting;
- protect messages in different ways, for example, turn on / off encryption / authentication;
- split messages into parts (packets), protect and transmit each part almost independently;
- set message recipients and provide them with key material in different ways;

### Versions

Many names have been used in the past, either by the Iota Foundation or by the community, to refer to two iterations of the MAM project. You may have heard, for example: MAM prototype, MAM, MAM0, MAM1, MAM2, MAM+...

As of now, the name MAM is referring to this current implementation.

### Layers

MAM uses a set of interconnected algorithms called layers. The following table is a quick definition of the main layers, for a more in-depth description of their states and algorithms, read [spec.pdf](https://github.com/iotaledger/entangled/blob/develop/mam/spec.pdf).

| Name | Description | Dependencies | Sources |
|------|-------------|--------------|---------|
| API | The API layer supports high-level operations that translates MAM messages to/from IOTA bundles. | MAM | [/mam/api](https://github.com/iotaledger/entangled/tree/develop/mam/api) |
| MAM | The MAM layer supports high-level operations of the MAM protocol like wrapping and unwrapping messages. | MSS, NTRU, PB3, PRNG, PSK | [/mam/mam](https://github.com/iotaledger/entangled/tree/develop/mam/mam) |
| MSS | The MSS layer supports Merkle-tree Signature Scheme. | Sponge, WOTS | [/mam/mss](https://github.com/iotaledger/entangled/tree/develop/mam/mss) |
| NTRU | The NTRU layer supports an NTRU-style public key encryption scheme. | PRNG, Sponge | [/mam/ntru](https://github.com/iotaledger/entangled/tree/develop/mam/ntru) |
| PB3 | The Protobuf3 layer supports encoding, decoding and cryptographic processing of structured data. | Sponge | [/mam/pb3](https://github.com/iotaledger/entangled/tree/develop/mam/pb3) |
| PRNG | The PRNG layer supports the generation of cryptographically strong pseudorandom array of trits. | Sponge | [/mam/prng](https://github.com/iotaledger/entangled/tree/develop/mam/prng) |
| PSK | The PSK layer supports Pre-Shared Key generation. | | [/mam/psk](https://github.com/iotaledger/entangled/tree/develop/mam/psk) |
| Sponge | The Sponge layer implements Authenticated Encryption and Hash using sponge construction. | Troika | [/mam/sponge](https://github.com/iotaledger/entangled/tree/develop/mam/sponge) |
| Troika | The troika layer supports a trinary hash function used as underlying function of the sponge layer. | | [/mam/troika](https://github.com/iotaledger/entangled/tree/develop/mam/troika) |
| WOTS | The WOTS layer supports Winternitz One-Time Signatures. | PRNG, Sponge | [/mam/wots](https://github.com/iotaledger/entangled/tree/develop/mam/wots) |

## Transport over the Tangle

MAM uses IOTA bundles as transport layer. It must be paired with a client library to actually receive and send them over the Tangle.

### Anatomy of a MAM message

## How to use MAM API ?

The following is a step by step guide to help you understand and use the core features of MAM.

You can follow it by copying the code snippets step by step. The full example is available [here](https://github.com/iotaledger/entangled/blob/develop/mam/examples/readme.c) and you can try it with the command `bazel run -c opt //mam/examples:readme`.

MAM is a library that allows you to read and write bundles. Sending and receiving bundles to and from the Tangle is out of the scope of MAM so we won't cover that part. To send and receive bundles in C, please refer to [cclient](https://github.com/iotaledger/entangled/tree/develop/cclient).

### Create a seed

To start using MAM, you will need a seed. If you don't know what a seed is or how to generate one, please refer to the [Glossary](#glossary).

With MAM, the following keys will be derived from your seed:
- Session keys;
- NTRU private keys and corresponding public keys;
- Pre-Shared Keys;
- MSS private keys and corresponding public keys;

For the sake of the example, let's say you generated the seed:
```c
tryte_t *seed = (tryte_t*)"MAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLE9";
```

> **WARNING**: Avoid using your regular token seed for MAM!

> **WARNING**: Do not share your seed to anyone!

### Initialize the API

```c
retcode_t mam_api_init(mam_api_t *const api, tryte_t const *const seed);
```

We begin by declaring and initializing an API instance.
```c
mam_api_t api;

mam_api_init(&api, seed);
```

MAM being a stateful library, this API instance will be holding your MAM account information during execution.

The state if composed of:
- Your channels and their endpoints;
- Your own NTRU private keys;
- NTRU public keys of your peers;
- Pre-Shared Keys;
- Contexts of messages you are sending;
- Contexts of messages you are receiving;

### Create a channel

```c
retcode_t mam_api_create_channel(mam_api_t *const api, size_t const height, tryte_t *const channel_id);
```

We create a new MAM channel by providing:
- the desired height of the underlying merkle tree which will determine how many signatures we can generate with this channel central endpoint;
- a tryte array to store the new channel ID which is the address where messages created through this channel will be sent to;

Here we choose a height of 5, meaning that we will be able to sign 2^5 = 32 packets with this channel central endpoint.

```c
tryte_t channel_id[MAM_CHANNEL_ID_SIZE];

mam_api_create_channel(&api, 5, channel_id);
```

`channel_id` should be equal to `CE9ITOJZBNDXOPU9F9WLOHYICXSAJDAQIINCIS9BQRWHWRLHQXDSXRUIINVQLBUIEZZNXKHTMLWYOKWPB`.

### Create an endpoint

```c
retcode_t mam_api_create_endpoint(mam_api_t *const api, size_t const height, tryte_t const *const channel_id, tryte_t *const endpoint_id);
```

We create a new MAM endpoint that belongs to the previously created channel by providing:
- the desired height of the underlying merkle tree which will determine how many signatures we can generate with this endpoint;
- the channel ID;
- a tryte array to store the new endpoint ID;

Here we choose a height of 5, meaning that we will be able to sign 2^5 = 32 packets with this endpoint.

```c
tryte_t endpoint_id[MAM_ENDPOINT_ID_SIZE];

mam_api_create_endpoint(&api, 5, channel_id, endpoint_id);
```

`endpoint_id` should be equal to `JHAMIP9PTLUTLRCKSYBTPDWIGTAVXFMJW9FYINDBXYIYJPLILUGGMZCSSSFWYUX9YCJZNOYUR9K9TALEA`.

Messages created through this endpoint will still be sent to the address of the channel, the channel ID.

### Write a header to a bundle

<!-- TODO NTRU / PSK -->

```c
retcode_t mam_api_bundle_write_header_on_channel(mam_api_t *const api, tryte_t const *const ch_id, mam_psk_t_set_t psks, mam_ntru_pk_t_set_t ntru_pks, bundle_transactions_t *const bundle, trit_t *const msg_id);
retcode_t mam_api_bundle_write_header_on_endpoint(mam_api_t *const api, tryte_t const *const ch_id, tryte_t const *const ep_id, mam_psk_t_set_t psks, mam_ntru_pk_t_set_t ntru_pks, bundle_transactions_t *const bundle, trit_t *const msg_id);
```

Now that we have a channel and an endpoint, we are ready to initiate our first message by writing a header to a bundle. We can do that directly through the central endpoint of the channel, or through the created endpoint. In both cases, the message will be sent to the channel address.

We need to provide:
- the channel ID;
- the endpoint ID (only when using the endpoint);
- a set of Pre-Shared Keys;
- a set of recipient NTRU public keys;
- a new bundle;
- a trit array to store the new message ID. This message ID uniquely identifies the message inside the channel and will be needed to write packets in the message;

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

At this point, the bundle can already be sent to the Tangle. You can also add the first packet of the message to this bundle before sending it, this is what the example will show.

### Write a packet to a bundle

```c
retcode_t mam_api_bundle_write_packet(mam_api_t *const api, trit_t const *const msg_id, tryte_t const *const payload, size_t const payload_size, mam_msg_checksum_t checksum, bool is_last_packet, bundle_transactions_t *const bundle);
```

We are now going to write the first (and only) packet of the message to the same bundle that already contains the header.

We need to provide:
- the message ID that uniquely identifies the message;
- a payload and its size, they both need to be in trytes. If your data is not in trytes (ASCII, trits, bytes...), you will first need to convert it using one of the functions provided [here](https://github.com/iotaledger/entangled/tree/develop/common/trinary);
- a checksum method between none, MAC and signature;
- a boolean value to tell if this is an intermediate packet (`false`) or the last packet of the message (`true`). If this is the last packet, all contexts related to this message will be removed on the sender side as well as the receiver side and it won't be possible to write new packets to this message anymore;
- the bundle you wish to write the packet into;

With no checksum.
```c
mam_api_bundle_write_packet(&api, message_id, (tryte_t *)"PAYLOAD", 7, MAM_MSG_CHECKSUM_NONE, true, bundle);
```

With MAC checksum you add integrity to your packet. Due to the underlying sponge construction of a MAM message, a MAC checksum actually also adds integrity to all previous packets of the message. Adding a MAC checksum to a packet is a really cheap operation so it is recommended in most cases.
```c
mam_api_bundle_write_packet(&api, message_id, (tryte_t *)"PAYLOAD", 7, MAM_MSG_CHECKSUM_MAC, true, bundle);
```

With signature (chosen method in the example) you add integrity and authenticity to your packet. The integrity part is due to the fact that the signed hash is actually the MAC checksum. For the same reasons, by signing a packet you add authentication to all previous packets of the message. Signing a packet is an expensive operation, depending on your use case you could for example sign one packet out of ten.
```c
mam_api_bundle_write_packet(&api, message_id, (tryte_t *)"PAYLOAD", 7, MAM_MSG_CHECKSUM_SIG, true, bundle);
```

We now have a MAM bundle containing a full message (header + packet) ready to be sent and received to and from the Tangle.

### Read a packet from a bundle

```c
retcode_t mam_api_bundle_read(mam_api_t *const api, bundle_transactions_t const *const bundle, tryte_t **const payload, size_t *const payload_size, bool *const is_last_packet);
```

Now let's imagine we are on the receiver side, we just fetched a bundle from the Tangle and we are ready to read it. It doesn't matter if the bundle contains only a header, a header and a first packet or only a packet, there is only one function to call.

We need to provide:
- the bundle;
- a payload that will either be filled with the read payload or set to `NULL` if the bundle contained only a header;
- a payload size that will either be set to the read payload size or to `0` if the bundle contained only a header;
- a boolean that will either be set to `true` if the packet was the last of the message or `false` otherwise;

```c
tryte_t *payload = NULL;
size_t payload_size = 0;
bool is_last_packet = false;

mam_api_bundle_read(&api, bundle, &payload, &payload_size, &is_last_packet);
free(payload);
```

> **WARNING**: The payload being allocated by the function, it must released when you're done with it.

### Save and load the API

```c
size_t mam_api_serialized_size(mam_api_t const *const api);
void mam_api_serialize(mam_api_t const *const api, trit_t *const buffer, tryte_t const *const encr_key_trytes, size_t encr_key_trytes_size);
retcode_t mam_api_deserialize(trit_t const *const buffer, size_t const buffer_size, mam_api_t *const api, tryte_t const *const decr_key_trytes, size_t decr_key_trytes_size);
retcode_t mam_api_save(mam_api_t const *const api, char const *const filename, tryte_t const *const encr_key_trytes, size_t encr_key_trytes_size);
retcode_t mam_api_load(char const *const filename, mam_api_t *const api, tryte_t const *const decr_key_trytes, size_t decr_key_trytes_size);
```

MAM being a stateful library, we need to keep the state alive from one execution to the other.

By serializing and deserializing the state to and from trits, we are able to use any kind of storage layer.

```c
mam_api_t new_api;
size_t serialized_size = mam_api_serialized_size(&api);
trit_t *buffer = malloc(serialized_size * sizeof(trit_t));

mam_api_serialize(&api, buffer, (tryte_t *)"ENCRYPTIONKEY", 13);
mam_api_deserialize(buffer, serialized_size, &new_api, (tryte_t *)"ENCRYPTIONKEY", 13);
free(buffer);
```

Convenient functions to save and load the state to and from a file are also available (chosen method in  the example).
```c
mam_api_t new_api;

mam_api_save(&api, "mam.state", (tryte_t *)"ENCRYPTIONKEY", 13);
mam_api_load("mam.state", &new_api, (tryte_t *)"ENCRYPTIONKEY", 13);
```

Both versions take an ecryption key to encrypt/decrypt the state. You can choose to not encrypt/decrypt it by providing a `NULL` pointer and/or a 0 size to them.

### Destroy the API

```c
retcode_t mam_api_destroy(mam_api_t *const api);
```

When you are done using MAM, you must destroy the API to release resources it was using.

```c
mam_api_destroy(&api);
```

## Security considerations

describe how keys should be generated, stored, distributed, destroyed; describe threats in case these operations are carried out incorrectly.

- Add warning: The API instance should not be lost; although it can be partially restored from the Seed, the message contexts will be lost and reusing restored API instance from the Seed can lead to key reuse and MAM keys can be compromised.
