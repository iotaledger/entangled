/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_PROTOTYPE_MAM_H__
#define __MAM_PROTOTYPE_MAM_H__

#include "common/crypto/curl-p/hamming.h"
#include "common/trinary/trit_long.h"
#include "mam/prototype/mask.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the encryption/decryption state for a MAM session
 *
 * @param side_key The encryption key
 * @param side_key_length The size of the encryption key
 * @param merkle_root The merkle root
 * @param enc_curl A curl instance used for encryption/decryption
 */
void mam_init_encryption(trit_t const *const side_key, size_t const side_key_length, trit_t const *const merkle_root,
                         Curl *const enc_curl);

/**
 * Computes the minimum length of a payload
 *
 * @param message_length The length of the message
 * @param merkle_tree_length The length of the merkle tree
 * @param index The leaf index
 * @param security The security used to generate addresses
 *
 * @return The minimum length of the payload
 */
int payload_min_length(size_t const message_length, size_t const merkle_tree_length, size_t const index,
                       size_t const security);

/**
 * Creates a signed, encrypted payload from a message
 *
 * @param payload The payload to be filled - Must be allocated
 * @param payload_length The payload length
 * @param message The message
 * @param message_length The message length
 * @param side_key The encryption key
 * @param side_key_length The length of the encryption key
 * @param merkle_tree The merkle tree
 * @param merkle_tree_length The length of the merkle tree
 * @param leaf_count The number of leaves of the merkle tree
 * @param index The leaf index
 * @param next_root The merkle root of the next MAM payload
 * @param start The offset used to generate addresses
 * @param seed The seed used to generate addresses - Not sent over the network
 * @param security - The security used to generate addresses
 * @param enc_curl A curl instance used for encryption/decryption
 *
 * @return Success/error code
 */
int mam_create(trit_t *const payload, size_t const payload_length, trit_t const *const message,
               size_t const message_length, trit_t const *const side_key, size_t const side_key_length,
               trit_t const *const merkle_tree, size_t const merkle_tree_length, size_t const leaf_count,
               size_t const index, trit_t const *const next_root, size_t const start, trit_t const *const seed,
               size_t const security, Curl *const enc_curl);

/**
 * Decrypts, parses and validates an encrypted payload
 *
 * @param payload The payload
 * @param payload_length The length of the payload
 * @param message The message to be filled - Must be allocated
 * @param message_length The length of the message
 * @param side_key The decryption key
 * @param side_key_length The length of the decryption key
 * @param root The merkle root
 * @param index The leaf index
 * @param next_root The merkle root of the next MAM payload
 * @param security The security used to generate addresses
 * @param enc_curl A curl instance used for encryption/decryption
 *
 * @return Success/error code
 */
int mam_parse(trit_t *const payload, size_t const payload_length, trit_t *const message, size_t *const message_length,
              trit_t const *const side_key, size_t const side_key_length, trit_t const *const root, size_t *const index,
              trit_t *const next_root, size_t *const security, Curl *const enc_curl);

#ifdef __cplusplus
}
#endif

#endif  //__MAM_PROTOTYPE_MAM_H__
