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
\file prng.h
\brief MAM2 PRNG layer.
*/
#ifndef __MAM_V2_PRNG_PRNG_H__
#define __MAM_V2_PRNG_PRNG_H__

#include "mam/v2/defs.h"
#include "mam/v2/err.h"
#include "mam/v2/sponge/sponge.h"
#include "mam/v2/trits/trits.h"

/*! \brief PRNG key size. */
#define MAM2_PRNG_KEY_SIZE 243

typedef word_t prng_key_t[MAM2_WORDS(MAM2_PRNG_KEY_SIZE)];

/*! \brief PRNG AE destination tryte. */
#define MAM2_PRNG_DST_SECKEY 0
/*! \brief PRNG WOTS destination tryte. */
#define MAM2_PRNG_DST_WOTSKEY 1
/*! \brief PRNG NTRU destination tryte. */
#define MAM2_PRNG_DST_NTRUKEY 2

/*! \brief PRNG interface. */
typedef struct prng_s {
  sponge_t *sponge; /*!< sponge interface */
  word_t *key;      /*!< key */
} prng_t;

/*! \brief PRNG initialization. */
void prng_init(prng_t *p,   /*!< [in] PRNG interface */
               sponge_t *s, /*!< [in] Sponge interface */
               trits_t K    /*!< [in] key of size MAM2_PRNG_KEY_SIZE */
);

/*! \brief PRNG output generation. */
void prng_gen(prng_t *p,  /*!< [in] PRNG interface */
              trint3_t d, /*!< [in] destination tryte */
              trits_t N,  /*!< [in] nonce */
              trits_t Y   /*!< [out] pseudorandom trits */
);

/*! \brief PRNG output generation with
nonce `N1` || `N2`. */
void prng_gen2(prng_t *p,  /*!< [in] PRNG interface */
               trint3_t d, /*!< [in] destination tryte */
               trits_t N1, /*!< [in] first nonce */
               trits_t N2, /*!< [in] second nonce */
               trits_t Y   /*!< [out] pseudorandom trits */
);

/*! \brief PRNG output generation with
nonce `N1` || `N2` || `N3`. */
void prng_gen3(prng_t *p,  /*!< [in] PRNG interface */
               trint3_t d, /*!< [in] destination tryte */
               trits_t N1, /*!< [in] first nonce */
               trits_t N2, /*!< [in] second nonce */
               trits_t N3, /*!< [in] third nonce */
               trits_t Y   /*!< [out] pseudorandom trits */
);

/*! \brief PRNG output generation. */
void prng_gen_str(prng_t *p,         /*!< [in] PRNG interface */
                  trint3_t d,        /*!< [in] destination tryte */
                  char const *nonce, /*!< [in] nonce */
                  trits_t Y          /*!< [out] pseudorandom trits */
);

/*! \brief Allocate memory for PRNG secret key. */
err_t prng_create(ialloc *a, prng_t *p);

/*! \brief Deallocate memory for PRNG secret key. */
void prng_destroy(ialloc *a, prng_t *p);

#endif  // __MAM_V2_PRNG_PRNG_H__
