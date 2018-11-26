/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 [ITSec Lab]
 *
 *
 * Refer to the LICENSE file for licensing information
 */

/*!
\file buffers.c
\brief Trits collection to simplify buffered operations.
*/

#include "mam/v2/buffers.h"
#include "utils/macros.h"

buffers_t buffers_init(size_t n, trits_t *Xs) {
  buffers_t bs = {trits_null(), n, Xs};
  return bs;
}

bool buffers_is_empty(buffers_t bs) {
  return trits_is_empty(bs.X) && (0 == bs.n);
}

size_t buffers_size(buffers_t bs) {
  size_t m = 0;

  for (; !buffers_is_empty(bs);) {
    m += trits_size(bs.X);
    bs.X = trits_null();

    if (0 < bs.n) {
      bs.X = *bs.Xs++;
      bs.n--;
    }
  }

  return m;
}

size_t buffers_copy_to(buffers_t *bs, trits_t buf) {
  size_t k, m = 0;

  for (; !trits_is_empty(buf) && !buffers_is_empty(*bs);) {
    k = MIN(trits_size(bs->X), trits_size(buf));
    trits_copy(trits_take(bs->X, k), trits_take(buf, k));
    bs->X = trits_drop(bs->X, k);
    buf = trits_drop(buf, k);
    m += k;

    if (trits_is_empty(bs->X) && (0 < bs->n)) {
      bs->X = *bs->Xs++;
      bs->n--;
    }
  }

  return m;
}

/* size_t buffers_copy_from(buffers_t *bs, trits_t buf) {
    size_t k, m = 0;

    for (; !trits_is_empty(buf) && !buffers_is_empty(*bs);) {
        k = trits_copy_min(buf, bs->X);
        bs->X = trits_drop(bs->X, k);
        buf = trits_drop(buf, k);
        m += k;

        if (trits_is_empty(bs->X) && (0 < bs->n)) {
            bs->X = *bs->Xs++;
            bs->n--;
        }
    }

    return m;
}*/
