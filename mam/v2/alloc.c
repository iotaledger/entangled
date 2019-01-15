/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

/*!
\file alloc.c
\brief MAM2 memory allocator.
*/
#include <stdlib.h>

#include "mam/v2/alloc.h"

void *mam_alloc(ialloc *a, size_t s) { return malloc(s); }

void mam_free(ialloc *a, void *p) {
  if (p) free(p);
}

word_t *mam_words_alloc(ialloc *a, size_t wc) {
  return (word_t *)mam_alloc(a, sizeof(word_t) * wc);
}

void mam_words_free(ialloc *a, word_t *ws) { mam_free(a, (void *)ws); }
