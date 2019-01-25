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
\file sponge.h
\brief MAM2 Sponge layer.
*/
#ifndef __MAM_V2_SPONGE_SPONGE_H__
#define __MAM_V2_SPONGE_SPONGE_H__

#include "mam/v2/defs.h"
#include "mam/v2/trits/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Sponge state rate. */
#define MAM2_SPONGE_RATE 486
/*! \brief Sponge state control. */
#define MAM2_SPONGE_CONTROL 6
/*! \brief Sponge state capacity. */
#define MAM2_SPONGE_CAPACITY 237
/*! \brief Sponge state width. */
#define MAM2_SPONGE_WIDTH \
  (MAM2_SPONGE_RATE + MAM2_SPONGE_CONTROL + MAM2_SPONGE_CAPACITY)

/*! \brief Sponge fixed key size. */
#define MAM2_SPONGE_KEY_SIZE 243
/*! \brief Sponge fixed hash size. */
#define MAM2_SPONGE_HASH_SIZE 243
/*! \brief Sponge fixed MAC size. */
#define MAM2_SPONGE_MAC_SIZE 243

/*! \brief Sponge state. */
typedef trit_t sponge_state_t[MAM2_SPONGE_WIDTH];

/*! \brief c2 control trit DATA */
#define MAM2_SPONGE_CTL_DATA 0
/*! \brief c2 control trit HASH */
#define MAM2_SPONGE_CTL_HASH 0
/*! \brief c2 control trit KEY */
#define MAM2_SPONGE_CTL_KEY 1
/*! \brief c2 control trit PRN */
#define MAM2_SPONGE_CTL_PRN 1
/*! \brief c2 control trit TEXT */
#define MAM2_SPONGE_CTL_TEXT -1
/*! \brief c2 control trit MAC */
#define MAM2_SPONGE_CTL_MAC -1

/**
 * Sponge interface.
 *
 * @field f sponge transformation
 * @field stack Additional memory used by `f`
 * @field state sponge state
 */

typedef struct sponge_s {
  void (*f)(void *, trit_t *); /*!< sponge transformation */
  void *stack;                 /*!< additional memory used by `f` */
  trit_t *state;               /*!< sponge state */
} sponge_t;

void sponge_transform(sponge_t *sponge);

/**
 * Fork (copy) sponge state. `fork` must be initialized.
 *
 * @param sponge [in] sponge interface
 * @param fork [out] sponge interface
 */
void sponge_fork(sponge_t *sponge, sponge_t *fork);

/**
 * Get part (the first `rate` trits) of the sponge outer state.
 *
 * @param sponge [in] sponge interface
 */
trits_t sponge_outer_trits(sponge_t *sponge);

/**
 * Sponge state initialization.
 *
 * @param sponge [in, out] sponge interface
 */
void sponge_init(sponge_t *sponge);

/**
 * Sponge absorption.
 *
 * @param sponge [in, out] sponge interface
 * @param c2 [in] control trit encoding output data type
 * @param data [in] input data blocks
 */
void sponge_absorb(sponge_t *sponge, trit_t c2, trits_t data);

/**
 * Absorb concatenation of `Xs[0]`..`Xs[n-1]`.
 *
 * @param sponge [in, out] sponge interface
 * @param c2 [in] control trit encoding output data type
 * @param n [in] input data blocks count
 * @param data_blocks [in] input data blocks
 */
void sponge_absorbn(sponge_t *sponge, trit_t c2, size_t n,
                    trits_t const *const data_blocks);

/**
 * Sponge squeezing.
 *
 * @param sponge [in] sponge interface
 * @param c2 [in] control trit encoding output data type
 * @param squeezed [out] output data
 */
void sponge_squeeze(sponge_t *sponge, trit_t c2, trits_t squeezed);

/**
 * Sponge AE encryption..
 *
 * @param sponge [in] sponge interface
 * @param plaintext [in] input data
 * @param ciphertext [out] hash value
 */
void sponge_encr(sponge_t *sponge, trits_t plaintext, trits_t ciphertext);

/**
 * Sponge AE decryption..
 *
 * @param sponge [in] sponge interface
 * @param ciphertext [in] hash value
 * @param plaintext [out] input data
 */
void sponge_decr(sponge_t *sponge, trits_t ciphertext, trits_t plaintext);

/**
 * Sponge hashing.
 *
 * @param sponge [in] sponge interface
 * @param plaintext [in] input data
 * @param digest [out] hash value
 */
void sponge_hash(sponge_t *sponge, trits_t plaintext, trits_t digest);

/**
 * Sponge hashing.
 *
 * @param sponge [in] sponge interface
 * @param n [in] input data blocks count
 * @param plaintext_blocks [in] input data blocks
 * @param digest [out] hash value
 */
void sponge_hashn(sponge_t *sponge, size_t n, trits_t *plaintext_blocks,
                  trits_t digest);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_SPONGE_SPONGE_H__
