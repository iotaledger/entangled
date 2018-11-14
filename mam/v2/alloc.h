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

/*! \brief MAM2 allocator type; nothing for now. */
typedef void ialloc;

void *mam2_alloc(ialloc *a, size_t s);

void mam2_free(ialloc *a, void *p);

trit_t *mam2_words_alloc(ialloc *a, size_t wc);

void mam2_words_free(ialloc *a, trit_t *ws);

#endif  // __MAM_V2_ALLOC_H_
