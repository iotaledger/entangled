/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_MAM_MAM_TYPES_H__
#define __MAM_V2_MAM_MAM_TYPES_H__

#include "common/errors.h"
#include "mam/v2/defs.h"
#include "mam/v2/trits/trits.h"

#define MAM2_PSK_ID_SIZE 81
#define MAM2_PSK_SIZE 243

#ifdef __cplusplus
extern "C" {
#endif

// Preshared key
typedef struct mam_pre_shared_key_s {
  trit_t id[MAM2_PSK_ID_SIZE];
  trit_t pre_shared_key[MAM2_PSK_SIZE];
} mam_pre_shared_key_t;

typedef struct mam_pre_shared_key_t_set_entry_s
    mam_pre_shared_key_t_set_entry_t;
typedef mam_pre_shared_key_t_set_entry_t* mam_pre_shared_key_t_set_t;

size_t psks_serialized_size(mam_pre_shared_key_t_set_t const psks);
retcode_t psks_serialize(mam_pre_shared_key_t_set_t const psks, trits_t trits);
retcode_t psks_deserialize(trits_t const trits,
                           mam_pre_shared_key_t_set_t* const psks);

// NTRU public key - 3g(x)/(1+3f(x)) - size
#define MAM2_NTRU_PK_SIZE 9216
// NTRU private key - f(x) - size
#define MAM2_NTRU_SK_SIZE 1024
// NTRU session symmetric key size
#define MAM2_NTRU_KEY_SIZE MAM2_SPONGE_KEY_SIZE
// NTRU encrypted key size
#define MAM2_NTRU_EKEY_SIZE 9216
// NTRU id size
#define MAM2_NTRU_ID_SIZE 81

// Recipient's NTRU public key
typedef struct mam_ntru_pk_s {
  trit_t pk[MAM2_NTRU_PK_SIZE];
} mam_ntru_pk_t;

typedef struct mam_ntru_pk_t_set_entry_s mam_ntru_pk_t_set_entry_t;
typedef mam_ntru_pk_t_set_entry_t* mam_ntru_pk_t_set_t;

size_t ntru_pks_serialized_size(mam_ntru_pk_t_set_t const ntru_pk_set);
retcode_t ntru_pks_serialize(mam_ntru_pk_t_set_t const ntru_pk_set,
                             trits_t trits);
retcode_t ntru_pks_deserialize(trits_t const trits,
                               mam_ntru_pk_t_set_t* const ntru_pk_set);

// NTRU layer interface
typedef struct mam_ntru_s {
  // Key id - the first 27 trytes of the corresponding public key
  trit_t public_key_id[MAM2_NTRU_ID_SIZE];
  // Private key trits - small coefficients of polynomial f
  trit_t secret_key[MAM2_NTRU_SK_SIZE];
  // Internal representation (`poly_t`) of a private key: NTT(1+3f)
  void* f;
} mam_ntru_t;

// NTRU layer interface
typedef struct mam_ntru_sk_s {
  // Key id - the first 27 trytes of the corresponding public key
  trit_t public_key_id[MAM2_NTRU_ID_SIZE];
  // Private key trits - small coefficients of polynomial f
  trit_t secret_key[MAM2_NTRU_SK_SIZE];
} mam_ntru_sk_t;

typedef struct mam_ntru_sk_t_set_entry_s mam_ntru_sk_t_set_entry_t;
typedef mam_ntru_sk_t_set_entry_t* mam_ntru_sk_t_set_t;

size_t mam_ntru_sk_serialized_size(mam_ntru_sk_t_set_t const ntru_sk_set);
retcode_t mam_ntru_sk_serialize(mam_ntru_sk_t_set_t const ntru_sk_set,
                                trits_t trits);
retcode_t mam_ntru_sk_deserialize(trits_t const trits,
                                  mam_ntru_sk_t_set_t* const ntru_sk_set);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_MAM_MAM_TYPES_H__
