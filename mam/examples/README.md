**PLEASE CREATE A NEW SEED WHILE  TRYING THESE EXAMPLES**

# Examples

These examples only serve the purpose of demonstrating the core features of MAM and are not as flexible as the library permits.

They will allow you to send and receive payloads from bundles.

Listening for new bundles is out of the scope of these examples.

For the proper execution of the examples, a file will be created under `/tmp/mam.bin`.

This file will be bound to a seed. If you wish to change seed, delete the file.

## Disclaimer

MAM is still under development, undefined behaviour can arise.

If so, please file an issue: https://github.com/iotaledger/entangled/issues/new.

**DO NOT USE YOUR USUAL TOKENS SEED**

## Running


### Sending

This command will send a header meaning it will declare a new message.

`bazel run -c opt -- //mam/examples:send-header <host> <port> <seed> <msg_public_key_type>`

This command will send a packet to an already existing message.

`bazel run -c opt -- //mam/examples:send-packet <host> <port> <seed> <msg_id> <payload> <last_packet>`

This command will send a header and a packet at the same time.

`bazel run -c opt -- //mam/examples:send-msg <host> <port> <seed> <payload> <last_packet>`

### Receiving

`bazel run -c opt -- //mam/examples:recv <host> <port> <bundle> <chid> (optional)`
