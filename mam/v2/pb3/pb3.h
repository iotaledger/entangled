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

#include "mam/v2/defs.h"
#include "mam/v2/err.h"
#include "mam/v2/sponge/spongos.h"
#include "mam/v2/trits/trits.h"

trits_t pb3_trits_take(trits_t *b, size_t n);

/*
Protobuf3 primitive types
null, tryte, size_t, trytes, tryte [n]
*/

size_t pb3_sizeof_tryte();

void pb3_encode_tryte(tryte_t t, trits_t *b);

err_t pb3_decode_tryte(tryte_t *t, trits_t *b);

#define pb3_sizeof_oneof pb3_sizeof_tryte
#define pb3_encode_oneof pb3_encode_tryte
#define pb3_decode_oneof pb3_decode_tryte

#define pb3_sizeof_optional pb3_sizeof_tryte
#define pb3_encode_optional pb3_encode_tryte
#define pb3_decode_optional pb3_decode_tryte

size_t pb3_sizeof_trint();

void pb3_encode_trint(trint9_t t, trits_t *b);

err_t pb3_decode_trint(trint9_t *t, trits_t *b);

size_t pb3_sizeof_longtrint();

void pb3_encode_longtrint(trint18_t t, trits_t *b);

err_t pb3_decode_longtrint(trint18_t *t, trits_t *b);

size_t pb3_sizeof_sizet(size_t n);

void pb3_encode_sizet(size_t n, trits_t *b);

err_t pb3_decode_sizet(size_t *n, trits_t *b);

#define pb3_sizeof_repeated pb3_sizeof_sizet
#define pb3_encode_repeated pb3_encode_sizet
#define pb3_decode_repeated pb3_decode_sizet

size_t pb3_sizeof_ntrytes(size_t n);

void pb3_encode_ntrytes(trits_t ntrytes, trits_t *b);

err_t pb3_decode_ntrytes(trits_t ntrytes, trits_t *b);

#if 0
 size_t pb3_sizeof_trytes(size_t n);

 void pb3_encode_trytes(trits_t trytes, trits_t *b);

 err_t pb3_decode_trytes(trits_t *trytes, trits_t *b);

 err_t pb3_decode_trytes2(ialloc *a, trits_t *trytes, trits_t *b);
#endif

/*
Protobuf3 spongos modifier handling
*/

void pb3_wrap_absorb_tryte(spongos_t *s, trits_t *b, tryte_t t);
err_t pb3_unwrap_absorb_tryte(spongos_t *s, trits_t *b, tryte_t *t);
void pb3_wrap_absorb_trint(spongos_t *s, trits_t *b, trint9_t t);
err_t pb3_unwrap_absorb_trint(spongos_t *s, trits_t *b, trint9_t *t);
void pb3_wrap_absorb_longtrint(spongos_t *s, trits_t *b, trint18_t t);
err_t pb3_unwrap_absorb_longtrint(spongos_t *s, trits_t *b, trint18_t *t);
void pb3_wrap_absorb_sizet(spongos_t *s, trits_t *b, size_t t);
err_t pb3_unwrap_absorb_sizet(spongos_t *s, trits_t *b, size_t *t);

void pb3_wrap_absorb_ntrytes(spongos_t *s, trits_t *b, trits_t t);
err_t pb3_unwrap_absorb_ntrytes(spongos_t *s, trits_t *b, trits_t t);
void pb3_wrap_crypt_ntrytes(spongos_t *s, trits_t *b, trits_t t);
err_t pb3_unwrap_crypt_ntrytes(spongos_t *s, trits_t *b, trits_t t);
void pb3_wrap_squeeze_ntrytes(spongos_t *s, trits_t *b, size_t n);
err_t pb3_unwrap_squeeze_ntrytes(spongos_t *s, trits_t *b, size_t n);

void pb3_absorb_external_ntrytes(spongos_t *s, trits_t t);
void pb3_squeeze_external_ntrytes(spongos_t *s, trits_t t);

#endif  // __MAM_V2_PB3_PB3_H__
