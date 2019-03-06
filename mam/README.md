# MAM

MAM (Masked authenticated messaging) is a trinary messages encryption scheme
that allows for both confidentiality and authenticity based on an original
implementation & specification by apmi.bsu.by

MAM allows for:

- Message encryption via NTRU (public key encrytpion) and PSK (PRe shared key)

- Sending a message to multiple receivers, each having his/her own session key

- Authenticating a message either via MAC (Integrity) or via MSS signature (Integrity + Authenticity)

## MAM components (directories within this repo)

*API* - Translates MAM messages into Tangle's bundle

*Examples* - Naive implementation of MAM messaging over
            the Tangle that makes use of CClient (https://github.com/iotaledger/entangled/tree/develop/cclient)

*MAM* - Contains implementation of messages encryption/decryption

*MSS* - "Merkle Signature Scheme" - this implementation is used to create new channels/endpoints

*WOTS* - "Winternitz one time signatures" - the underlying signature implementation of the MSS layer

*NTRU* - Public key encryption scheme

*PSK* - pre shared key encryption (symmetric encryption)

*PRNG* - Pseudo randomness generator based on sponge's hash function

*PB3* - Encoding implementation used for serialization and by the MAM layer mostly

*Sponge* - Hash based cryptography based on the Keccack duplex construction (https://keccak.team/sponge_duplex.html)

*Troika* - Trinary hash function, it's transform function is used as the _Round_ function for the Sponge layer

*Trits* - utility functions for manipulating trit arrays

## How to use it?

Examples for writing/reading messages to/from the tangle are available here:
https://github.com/iotaledger/entangled/tree/develop/mam/examples