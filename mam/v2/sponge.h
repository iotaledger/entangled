/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_SPONGE_H__
#define __MAM_V2_SPONGE_H__

#include "common/trinary/trit_array.h"
#include "mam/v2/trits.h"

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
typedef trit_t sponge_state_t[MAM2_WORDS(MAM2_SPONGE_WIDTH)];

/*! \brief c2 control trit DATA */
#define MAM2_SPONGE_CTL_DATA 0
/*! \brief c2 control trit HASH */
#define MAM2_SPONGE_CTL_HASH 0
/*! \brief c2 control trit KEY */
#define MAM2_SPONGE_CTL_KEY 1
/*! \brief c2 control trit PRN */
#define MAM2_SPONGE_CTL_PRN 1

/*! \brief Sponge interface. */
typedef struct sponge_s {
  void (*f)(void *, trit_t *); /*!< sponge transformation */
  void *stack;                 /*!< additional memory used by `f` */
  trit_t *s;                   /*!< sponge state */
} sponge_t;

/*! \brief Fork (copy) sponge state. `fork` must be initialized. */
void sponge_fork(sponge_t *s, sponge_t *fork);

/*! \brief Get part (the first `n` trits) of the sponge outer state.
\note It is usually used with `sponge_absorb1`.
*/
trits_t sponge_outer_trits(sponge_t *s, size_t n);

/*! \brief Sponge state initialization. */
void sponge_init(sponge_t *s /*!< [in] sponge interface */
);

/*! \brief Sponge absorption. */
void sponge_absorb(sponge_t *s, /*!< [in] sponge interface */
                   trit_t c2, /*!< [in] control trit encoding input data type */
                   trits_t X  /*!< [in] input data */
);

void sponge_absorb_flex(sponge_t *s, trit_t c2, trit_array_p X_arr);

/*! \brief Absorb concatenation of `Xs[0]`..`Xs[n-1]` */
void sponge_absorbn_flex(
    sponge_t *s,      /*!< [in] sponge interface */
    trit_t c2,        /*!< [in] control trit encoding input data type */
    size_t n,         /*!< [in] input data blocks count */
    trit_array_t Xs[] /*!< [in] input data blocks */
);

/*! \brief Sponge squeezing. */
void sponge_squeeze(
    sponge_t *s, /*!< [in] sponge interface */
    trit_t c2,   /*!< [in] control trit encoding output data type */
    trits_t Y    /*!< [out] output data */
);

/*! \brief Sponge squeezing. */
void sponge_squeeze_flex(
    sponge_t *s,   /*!< [in] sponge interface */
    trit_t c2,     /*!< [in] control trit encoding output data type */
    trit_array_p Y /*!< [out] output data */
);

/*! \brief Sponge AE encryption. */
void sponge_encr(sponge_t *s, /*!< [in] sponge interface */
                 trits_t X,   /*!< [in] plaintext */
                 trits_t Y    /*!< [out] ciphertext */
);

/*! \brief Sponge AE encryption. */
void sponge_encr_flex(sponge_t *s,    /*!< [in] sponge interface */
                      trit_array_p X, /*!< [in] plaintext */
                      trit_array_p Y  /*!< [out] ciphertext */
);

/*! \brief Sponge AE decryption. */
void sponge_decr(sponge_t *s, /*!< [in] sponge interface */
                 trits_t X,   /*!< [in] ciphertext */
                 trits_t Y    /*!< [out] plaintext */
);

/*! \brief Sponge AE decryption. */
void sponge_decr_flex(sponge_t *s,    /*!< [in] sponge interface */
                      trit_array_p X, /*!< [in] ciphertext */
                      trit_array_p Y  /*!< [out] plaintext */
);

void sponge_hash(sponge_t *s,    /*!< [in] sponge interface */
                 trit_array_p X, /*!< [in] input data */
                 trit_array_p Y  /*!< [out] hash value */
);

void sponge_hashn(sponge_t *s,      /*!< [in] sponge interface */
                  size_t n,         /*!< [in] input data blocks count */
                  trit_array_t *Xs, /*!< [in] input data blocks */
                  trit_array_p Y    /*!< [out] hash value */

);

#endif  // __MAM_V2_SPONGE_H__
