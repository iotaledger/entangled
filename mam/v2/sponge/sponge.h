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
#pragma once

#include "mam/v2/defs.h"
#include "mam/v2/trits/trits.h"

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
typedef word_t sponge_state_t[MAM2_WORDS(MAM2_SPONGE_WIDTH)];

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

/*! \brief Sponge interface. */
typedef struct _isponge {
  void (*f)(void *, word_t *); /*!< sponge transformation */
  void *stack;                 /*!< additional memory used by `f` */
  word_t *s;                   /*!< sponge state */
} isponge;

void sponge_transform(isponge *s);

/*! \brief Fork (copy) sponge state. `fork` must be initialized. */
void sponge_fork(isponge *s, isponge *fork);

/*! \brief Get part (the first `rate` trits) of the sponge outer state.
\note It is usually used with `sponge_absorb1`.
*/
trits_t sponge_outer_trits(isponge *s);

/*! \brief Sponge state initialization. */
void sponge_init(isponge *s /*!< [in] sponge interface */
);

/*! \brief Sponge absorption. */
void sponge_absorb(isponge *s, /*!< [in] sponge interface */
                   trit_t c2, /*!< [in] control trit encoding input data type */
                   trits_t X  /*!< [in] input data */
);

/*! \brief Absorb concatenation of `Xs[0]`..`Xs[n-1]` */
void sponge_absorbn(
    isponge *s, /*!< [in] sponge interface */
    trit_t c2,  /*!< [in] control trit encoding input data type */
    size_t n,   /*!< [in] input data blocks count */
    trits_t *Xs /*!< [in] input data blocks */
);

/*! \brief Sponge squeezing. */
void sponge_squeeze(
    isponge *s, /*!< [in] sponge interface */
    trit_t c2,  /*!< [in] control trit encoding output data type */
    trits_t Y   /*!< [out] output data */
);

/*! \brief Sponge AE encryption. */
void sponge_encr(isponge *s, /*!< [in] sponge interface */
                 trits_t X,  /*!< [in] plaintext */
                 trits_t Y   /*!< [out] ciphertext */
);

/*! \brief Sponge AE decryption. */
void sponge_decr(isponge *s, /*!< [in] sponge interface */
                 trits_t X,  /*!< [in] ciphertext */
                 trits_t Y   /*!< [out] plaintext */
);

/*! \brief Sponge hashing. */
void sponge_hash(isponge *s, /*!< [in] sponge interface */
                 trits_t X,  /*!< [in] input data */
                 trits_t Y   /*!< [out] hash value */
);

/*! \brief Sponge hashing. */
void sponge_hashn(isponge *s,  /*!< [in] sponge interface */
                  size_t n,    /*!< [in] input data blocks count */
                  trits_t *Xs, /*!< [in] input data blocks */
                  trits_t Y    /*!< [out] hash value */
);

/*! \brief Sponge test (AE). */
bool_t sponge_test(isponge *s);
