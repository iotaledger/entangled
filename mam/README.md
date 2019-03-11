# MAM

MAM (Masked authenticated messaging) is a trinary messages encryption scheme
that allows for both confidentiality and authenticity based on an original
implementation & specification by apmi.bsu.by

MAM allows for:

- Message encryption via NTRU (public key encrytpion) and PSK (Pre-Shared Key)

- Sending a message to multiple receivers, each having his/her own session key

- Authenticating a message either via MAC (Integrity) or via MSS signature (Integrity + Authenticity)

## MAM components (directories within this repo)

*API* - Translates MAM messages into Tangle's bundle

*Examples* - Naive implementation of MAM messaging over
            the Tangle that makes use of CClient (https://github.com/iotaledger/entangled/tree/develop/cclient)

*MAM* - Contains implementation of messages encryption/decryption

*MSS* - "Merkle Signature Scheme" - this implementation is used to create new channels/endpoints

*WOTS* - "Winternitz one time signatures" - the underlying signature implementation of the MSS layer

*NTRU* - Supports an NTRU-style public key encryption scheme. Using NTRU a sender can encrypt session keys with a public key of a recipient. The secret key must be kept in secret. The corresponding public key, on the contrary, is publicly announced.

*PSK* - Pre-Shared Key (PSK) is a secret key of Authenticated Encryption (AE). It is preliminarily transmitted between the entities and is beyond the scope of MAM. The PSK id is an identifier of a group of recipients who share the same PSK.

*PRNG* - Pseudo randomness generator based on sponge's hash function

*PB3* - Encoding implementation used for serialization and by the MAM layer mostly

*Sponge* - Hash based cryptography based on the Keccack duplex construction (https://keccak.team/sponge_duplex.html)

*Troika* - Trinary hash function, it's transform function is used as the _Round_ function for the Sponge layer

*Trits* - utility functions for manipulating trit arrays

## How to use it?

### Managing PSKs

The sender creates a PSK by providing a 27-trytes id and a custom trytes nonce, then provides it to recipients.
```
mam_psk_t psk;
mam_psk_gen(&psk, &prng, "B9IOSRYXSJPELPKGTG9PJDQC9YS", "PZQZ...AKKEF", 42);
// Provides it to recipients
mam_psk_destroy(&psk);
```

The receiver receives a PSK from a sender and adds it to the API.
```
mam_psk_t psk;
// Receives it from sender
mam_api_add_psk(&api, &psk);
mam_psk_destroy(&psk);
```
