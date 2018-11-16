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
\file pb3.h
\brief MAM2 Protobuf3 layer.
*/
#ifndef __MAM_V2_PB3_H__
#define __MAM_V2_PB3_H__

#include "mam/v2//err.h"
#include "mam/v2/defs.h"
#include "mam/v2/sponge.h"
#include "trits.h"

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

size_t pb3_sizeof_trytes(size_t n);

void pb3_encode_trytes(trits_t trytes, trits_t *b);

err_t pb3_decode_trytes(trits_t *trytes, trits_t *b);

err_t pb3_decode_trytes2(trits_t *trytes, trits_t *b);

/*
Protobuf3 cryptographic modifier handling:
secret, encrypted, data(other)
*/

void pb3_wrap_secret(isponge *s, trits_t t);

void pb3_unwrap_secret(isponge *s, trits_t t);

void pb3_wrap_encrypted(isponge *s, trits_t t);

void pb3_unwrap_encrypted(isponge *s, trits_t t);

void pb3_wrap_data(isponge *s, trits_t t);

void pb3_unwrap_data(isponge *s, trits_t t);

bool_t pb3_test();

#endif  // __MAM_V2_PB3_H__
