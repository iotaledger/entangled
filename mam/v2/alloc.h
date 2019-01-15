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
\file alloc.h
\brief MAM2 memory allocator.
*/
#pragma once

#include "mam/v2/defs.h"

/*! \brief MAM2 allocator type; nothing for now. */
typedef void ialloc;

void *mam_alloc(ialloc *a, size_t s);

void mam_free(ialloc *a, void *p);

word_t *mam_words_alloc(ialloc *a, size_t wc);

void mam_words_free(ialloc *a, word_t *ws);
