/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by [ITSec Lab]
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
#ifndef __MAM_MSS_MSS_CLASSIC_H__
#define __MAM_MSS_MSS_CLASSIC_H__

#include "common/errors.h"
#include "mam/defs.h"
#include "mam/prng/prng.h"
#include "mam/sponge/spongos.h"
#include "mam/trits/trits.h"
#include "mam/wots/wots.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief MSS Merkle-tree implementation storage words. */
#define MAM_MSS_MT_WORDS(height) (MAM_WOTS_PUBLIC_KEY_SIZE * (2 * (1 << (height)) - 1))
#define MAM_MSS_MT_MAX_STORED_SIZE(height) (((1 << ((height) + 1)) - 1) * MAM_MSS_MT_HASH_SIZE)

/*! \brief MSS interface used to generate public key and sign. */
typedef struct mam_mss_s {
  mss_mt_height_t height;                 /*!< Merkle tree height. */
  mss_mt_idx_t skn;                       /*!< Current WOTS private key number. */
  mam_prng_t *prng;                       /*!< PRNG interface used to generate WOTS private keys. */
  trit_t *mt;                             /*!< Buffer storing complete Merkle-tree. */
  trits_t nonce1, nonce2, nonce3, nonce4; /*!< Nonce = `N1`||`N2`||`N3`||`N4`, stored pointers only, NOT copies. */
  trit_t root[MAM_MSS_PK_SIZE];
} mam_mss_t;

#ifdef __cplusplus
}
#endif

#endif  // __MAM_MSS_MSS_CLASSIC_H__

/** @} */
