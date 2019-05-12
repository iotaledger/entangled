/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup mam
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef __MAM_TRITS_BUFFERS_H__
#define __MAM_TRITS_BUFFERS_H__

#include "mam/defs.h"
#include "mam/trits/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct buffers_s {
  trits_t head;
  trits_t *tail;
  size_t count;
} buffers_t;

/**
 * Init buffers with empty head
 *
 * @param count The number of trit arrays
 * @param trits The trit arrays
 *
 * @return the buffers
 */
buffers_t buffers_init(size_t const count, trits_t const *const trits);

/**
 * Check whether head is empty and there is no tail
 * However if the tail buffers consist of empty buffers only it will _not_
 * be considered empty!
 *
 * @param buffers The buffers
 *
 * @return true if empty, false otherwise
 */
static inline bool buffers_is_empty(buffers_t const buffers) {
  return trits_is_empty(buffers.head) && (buffers.count == 0);
}

/**
 * Sum of head and tail buffers sizes
 *
 * @param buffers The buffers
 *
 * @return the size of the buffers
 */
size_t buffers_size(buffers_t buffers);

/**
 * Copy buffers `*buffers` to a destination `trits`
 *
 * @param buffers The buffers
 * @param trits The trits
 *
 * @return the number of trits copied
 */
size_t buffers_copy_to(buffers_t *const buffers, trits_t trits);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_TRITS_BUFFERS_H__

/** @} */
