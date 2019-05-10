/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/trits/buffers.h"

buffers_t buffers_init(size_t const count, trits_t const *const trits) {
  buffers_t buffers;

  buffers.head = trits_null();
  buffers.count = count;
  buffers.tail = (trits_t *const)trits;

  return buffers;
}

size_t buffers_size(buffers_t buffers) {
  size_t m = 0;

  for (; !buffers_is_empty(buffers);) {
    m += trits_size(buffers.head);
    buffers.head = trits_null();

    if (0 < buffers.count) {
      buffers.head = *buffers.tail++;
      buffers.count--;
    }
  }

  return m;
}

size_t buffers_copy_to(buffers_t *const buffers, trits_t trits) {
  size_t k, m = 0;

  for (; !trits_is_empty(trits) && !buffers_is_empty(*buffers);) {
    k = trits_copy_min(buffers->head, trits);
    buffers->head = trits_drop(buffers->head, k);
    trits = trits_drop(trits, k);
    m += k;

    if (trits_is_empty(buffers->head) && (0 < buffers->count)) {
      buffers->head = *buffers->tail++;
      buffers->count--;
    }
  }

  return m;
}
