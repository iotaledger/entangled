# MAM

## What is MAM ?

### MAMv0 ? MAMv1 ? MAMv2 ? MAM+ ?

A lot of names have been used in the past, either by the Iota Foundation or by the community, to refer to two different versions of MAM. As of now, the name `MAM` will refer to the current fully spec'd and fully revised implementation while `MAM prototype` will refer to the previous implementation.

### Definition

MAM (Masked authenticated messaging) is a trinary messages encryption scheme
that allows for both confidentiality and authenticity based on an original
implementation & specification by apmi.bsu.by

MAM allows for:

- Message encryption via NTRU (public key encrytpion) and PSK (Pre-Shared Key)

- Sending a message to multiple receivers, each having his/her own session key

- Authenticating a message either via MAC (Integrity) or via MSS signature (Integrity + Authenticity)

## MAM layers


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

For a more in-depth description of the layers, their states and their algorithms, read [spec.pdf](https://github.com/iotaledger/entangled/blob/develop/mam/spec.pdf).

## Anatomy of a MAM message

## How to use MAM ?

To start using MAM, you need a seed. To know how to generate a seed, refer to [create-a-seed](https://docs.iota.org/docs/getting-started/0.1/tutorials/create-a-seed).

> **WARNING**: Do not use your regular token seed for MAM !
