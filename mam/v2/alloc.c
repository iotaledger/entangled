
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
\file alloc.c
\brief MAM2 memory allocator.
*/

#include "mam/v2/alloc.h"

#include <stdlib.h>

void *mam2_alloc(ialloc *a, size_t s) { return malloc(s); }

void mam2_free(ialloc *a, void *p) {
  if (p) free(p);
}

word_t *mam2_words_alloc(ialloc *a, size_t wc) {
  return (word_t *)mam2_alloc(a, sizeof(word_t) * wc);
}

void mam2_words_free(ialloc *a, word_t *ws) { mam2_free(a, (void *)ws); }
