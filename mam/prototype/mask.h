/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_PROTOTYPE_MASK_H__
#define __MAM_PROTOTYPE_MASK_H__

#include "common/crypto/curl-p/trit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Masks a given message with a curl instance state
 *
 * @param dest Destination of the masked message
 * @param message Message to be masked
 * @param length Length of the message
 * @param c The curl instance used to mask the message
 */
void mask(trit_t *const dest, trit_t const *const message, size_t const length, Curl *const c);

/**
 * Unmasks a given cipher with a curl instance state
 *
 * @param dest Destination of the unmasked cipher
 * @param cipher Cipher to be unmasked
 * @param length Length of the cipher
 * @param c The curl instance used to unmask the cipher
 */
void unmask(trit_t *const dest, trit_t const *const cipher, size_t const length, Curl *const c);

#ifdef __cplusplus
}
#endif

#endif  //__MAM_PROTOTYPE_MASK_H__
