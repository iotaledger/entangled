
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

trit_t *mam2_words_alloc(size_t wc) {
  return (trit_t *)malloc(sizeof(trit_t) * wc);
}

void mam2_words_free(trit_t *ws) { free(ws); }
