/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
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
#ifndef __MAM_PB3_PB3_H__
#define __MAM_PB3_PB3_H__

#include "common/errors.h"
#include "mam/defs.h"
#include "mam/sponge/spongos.h"
#include "mam/trits/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

trits_t pb3_trits_take(trits_t *const b, size_t const n);

/**
 * Protobuf3 primitive types
 * null, tryte, size_t, trytes, tryte [n]
 */

size_t pb3_sizeof_tryte();
void pb3_encode_tryte(tryte_t const tryte, trits_t *const buffer);
retcode_t pb3_decode_tryte(tryte_t *const tryte, trits_t *const buffer);

#define pb3_sizeof_oneof pb3_sizeof_tryte

size_t pb3_sizeof_trint();
void pb3_encode_trint(trint9_t const trint, trits_t *const buffer);
retcode_t pb3_decode_trint(trint9_t *const trint, trits_t *const buffer);

size_t pb3_sizeof_longtrint();
void pb3_encode_longtrint(trint18_t const trint, trits_t *const buffer);
retcode_t pb3_decode_longtrint(trint18_t *const trint, trits_t *const buffer);

size_t pb3_sizeof_size_t(size_t const n);
void pb3_encode_size_t(size_t n, trits_t *const buffer);
retcode_t pb3_decode_size_t(size_t *const n, trits_t *const buffer);

#define pb3_sizeof_repeated pb3_sizeof_size_t

size_t pb3_sizeof_ntrytes(size_t const n);
void pb3_encode_ntrytes(trits_t const ntrytes, trits_t *const buffer);
retcode_t pb3_decode_ntrytes(trits_t const ntrytes, trits_t *const buffer);

/**
 * Protobuf3 spongos modifier handling
 */

void pb3_wrap_absorb_tryte(mam_spongos_t *const spongos, trits_t *const buffer, tryte_t const tryte);
retcode_t pb3_unwrap_absorb_tryte(mam_spongos_t *const spongos, trits_t *const buffer, tryte_t *const tryte);

void pb3_wrap_absorb_trint(mam_spongos_t *const spongos, trits_t *const buffer, trint9_t const trint);
retcode_t pb3_unwrap_absorb_trint(mam_spongos_t *const spongos, trits_t *const buffer, trint9_t *const trint);

void pb3_wrap_absorb_size_t(mam_spongos_t *const spongos, trits_t *const buffer, size_t const t);
retcode_t pb3_unwrap_absorb_size_t(mam_spongos_t *const spongos, trits_t *const buffer, size_t *const t);

void pb3_wrap_absorb_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, trits_t const trits);
retcode_t pb3_unwrap_absorb_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, trits_t const trits);
void pb3_wrap_crypt_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, trits_t const trits);
retcode_t pb3_unwrap_crypt_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, trits_t const trits);
void pb3_wrap_squeeze_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, size_t const n);
retcode_t pb3_unwrap_squeeze_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, size_t const n);

void pb3_absorb_external_ntrytes(mam_spongos_t *const spongos, trits_t const trits);
void pb3_squeeze_external_ntrytes(mam_spongos_t *const spongos, trits_t const trits);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_PB3_PB3_H__

/** @} */
