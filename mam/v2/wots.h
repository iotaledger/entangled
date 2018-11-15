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
\file wots.h
\brief MAM2 WOTS layer.
*/
#ifndef __MAM_V2_WOTS_H__
#define __MAM_V2_WOTS_H__

#include "mam/v2/defs.h"
#include "mam/v2/err.h"
#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"
#include "trits.h"

/*! \brief WOTS public key size. */
#define MAM2_WOTS_PK_SIZE 243
/*! \brief WOTS private key part size. */
#define MAM2_WOTS_SK_PART_SIZE 162
/*! \brief WOTS private key parts count. */
#define MAM2_WOTS_SK_PART_COUNT 81
/*! \brief WOTS private key size. */
#define MAM2_WOTS_SK_SIZE (MAM2_WOTS_SK_PART_SIZE * MAM2_WOTS_SK_PART_COUNT)

/*! \brief WOTS signed hash value size. */
#define MAM2_WOTS_HASH_SIZE 234
/*! \brief WOTS signature size. */
#define MAM2_WOTS_SIG_SIZE MAM2_WOTS_SK_SIZE

typedef trit_t wots_sk_t[MAM2_WORDS(MAM2_WOTS_SK_SIZE)];

/*! \brief WOTS interface. */
typedef struct _iwots {
  isponge *s; /*!< sponge interface */
  trit_t *sk; /*!< private key */
} iwots;

/*! \brief Init WOTS interface with Sponge. */
MAM2_API void wots_init(iwots *w,  /*!< [in,out] WOTS interface */
                        isponge *s /*!< [in] Sponge interface */
);

/*! \brief Generate WOTS private key. */
MAM2_API void wots_gen_sk(iwots *w, /*!< [in] WOTS interface */
                          iprng *p, /*!< [in] PRNG interface */
                          trits_t N /*!< [in] nonce */
);

/*! \brief Generate WOTS private key. */
MAM2_API void wots_gen_sk2(iwots *w,   /*!< [in] WOTS interface */
                           iprng *p,   /*!< [in] PRNG interface */
                           trits_t N1, /*!< [in] first nonce */
                           trits_t N2  /*!< [in] second nonce */
);

/*! \brief Generate WOTS private key. */
MAM2_API void wots_gen_sk3(iwots *w,   /*!< [in] WOTS interface */
                           iprng *p,   /*!< [in] PRNG interface */
                           trits_t N1, /*!< [in] first nonce */
                           trits_t N2, /*!< [in] second nonce */
                           trits_t N3  /*!< [in] third nonce */
);

/*! \brief Calculate WOTS public key.
\note Private key must have already been generated. */
MAM2_API void wots_calc_pk(iwots *w,  /*!< [in] WOTS interface */
                           trits_t pk /*!< [out] public key */
);

/*! \brief Generate WOTS signature. */
MAM2_API void wots_sign(iwots *w,   /*!< [in] WOTS interface */
                        trits_t H,  /*!< [in] hash value tbs */
                        trits_t sig /*!< [out] signature */
);

/*! \brief Recover WOTS public key from signature. */
MAM2_API void wots_recover(isponge *s,  /*!< [in] Sponge interface */
                           trits_t H,   /*!< [in] signed hash value */
                           trits_t sig, /*!< [in] signature */
                           trits_t pk   /*!< [out] presumed public key */
);

/*! \brief Verify WOTS signature. */
MAM2_API bool_t wots_verify(isponge *s,  /*!< [in] Sponge interface */
                            trits_t H,   /*!< [in] signed hash value */
                            trits_t sig, /*!< [in] signature */
                            trits_t pk   /*!< [in] public key */
);

/*! \brief Allocate memory for WOTS private key. */
MAM2_API err_t wots_create(iwots *w);

/*! \brief Deallocate memory for WOTS private key. */
MAM2_API void wots_destroy(iwots *w);

MAM2_API trits_t wots_sk_trits(iwots *w);

#endif  // __MAM_V2_WOTS_H__
