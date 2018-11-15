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
\file alloc.h
\brief MAM2 memory allocator.
*/

#ifndef __MAM_V2_ALLOC_H__
#define __MAM_V2_ALLOC_H__

#include "mam/v2/defs.h"

trit_t *mam2_words_alloc(size_t wc);

void mam2_words_free(trit_t *ws);

#endif  // __MAM_V2_ALLOC_H_
