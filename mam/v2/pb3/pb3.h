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
\file pb3.h
\brief MAM2 Protobuf3 layer.
*/
#ifndef __MAM_V2_PB3_PB3_H__
#define __MAM_V2_PB3_PB3_H__

#include "common/errors.h"
#include "mam/v2/defs.h"
#include "mam/v2/sponge/spongos.h"
#include "mam/v2/trits/trits.h"

/* \brief Maximal value of PB3 size_t value = (27^13-1)/2. */
#define MAM2_PB3_SIZE_MAX 2026277576509488133ULL

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
#define pb3_encode_oneof pb3_encode_tryte
#define pb3_decode_oneof pb3_decode_tryte

#define pb3_sizeof_optional pb3_sizeof_tryte
#define pb3_encode_optional pb3_encode_tryte
#define pb3_decode_optional pb3_decode_tryte

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
#define pb3_encode_repeated pb3_encode_size_t
#define pb3_decode_repeated pb3_decode_size_t

size_t pb3_sizeof_ntrytes(size_t const n);
void pb3_encode_ntrytes(trits_t const ntrytes, trits_t *const buffer);
retcode_t pb3_decode_ntrytes(trits_t const ntrytes, trits_t *const buffer);

/**
 * Protobuf3 spongos modifier handling
 */

void pb3_wrap_absorb_tryte(spongos_t *const spongos, trits_t *const buffer,
                           tryte_t const tryte);
retcode_t pb3_unwrap_absorb_tryte(spongos_t *const spongos,
                                  trits_t *const buffer, tryte_t *const tryte);

void pb3_wrap_absorb_trint(spongos_t *const spongos, trits_t *const buffer,
                           trint9_t const trint);
retcode_t pb3_unwrap_absorb_trint(spongos_t *const spongos,
                                  trits_t *const buffer, trint9_t *const trint);

void pb3_wrap_absorb_longtrint(spongos_t *const spongos, trits_t *const buffer,
                               trint18_t const trint);
retcode_t pb3_unwrap_absorb_longtrint(spongos_t *const spongos,
                                      trits_t *const buffer,
                                      trint18_t *const trint);

void pb3_wrap_absorb_size_t(spongos_t *const spongos, trits_t *const buffer,
                            size_t const t);
retcode_t pb3_unwrap_absorb_size_t(spongos_t *const spongos,
                                   trits_t *const buffer, size_t *const t);

void pb3_wrap_absorb_ntrytes(spongos_t *const spongos, trits_t *const buffer,
                             trits_t const trits);
retcode_t pb3_unwrap_absorb_ntrytes(spongos_t *const spongos,
                                    trits_t *const buffer, trits_t const trits);
void pb3_wrap_crypt_ntrytes(spongos_t *const spongos, trits_t *const buffer,
                            trits_t const trits);
retcode_t pb3_unwrap_crypt_ntrytes(spongos_t *const spongos,
                                   trits_t *const buffer, trits_t const trits);
void pb3_wrap_squeeze_ntrytes(spongos_t *const spongos, trits_t *const buffer,
                              size_t const n);
retcode_t pb3_unwrap_squeeze_ntrytes(spongos_t *const spongos,
                                     trits_t *const buffer, size_t const n);

void pb3_absorb_external_ntrytes(spongos_t *const spongos, trits_t const trits);
void pb3_squeeze_external_ntrytes(spongos_t *const spongos,
                                  trits_t const trits);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_PB3_PB3_H__
