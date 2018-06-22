#ifndef __MAM_MASK_H__
#define __MAM_MASK_H__

#include "common/curl-p/trit.h"

/**
 * Masks a given message with a curl instance state
 *
 * @param dest Destination of the masked message
 * @param message Message to be masked
 * @param length Length of the message
 * @param c The curl instance used to mask the message
 */
void mask(trit_t *dest, trit_t *message, size_t length, Curl *const c);

/**
 * Unmasks a given cipher with a curl instance state
 *
 * @param dest Destination of the unmasked cipher
 * @param cipher Cipher to be unmasked
 * @param length Length of the cipher
 * @param c The curl instance used to unmask the cipher
 */
void unmask(trit_t *dest, trit_t *cipher, size_t length, Curl *const c);

#endif  //__MAM_MASK_H__
