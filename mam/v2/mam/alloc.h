/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_MAM_ALLOC_H__
#define __MAM_V2_MAM_ALLOC_H__

#include "mam/v2/sponge/sponge.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * MAM allocator
 *
 * @field create_sponge Allocator for sponge interfaces used by
 * channels/endpoints (WOTS, PB3 sponge, PB3 fork sponge)
 * @field destroy_sponge Deallocator
 */
typedef struct mam_ialloc_s {
  mam_sponge_t *(*create_sponge)();
  void (*destroy_sponge)(mam_sponge_t *);
} mam_ialloc_t;

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_MAM_ALLOC_H__
