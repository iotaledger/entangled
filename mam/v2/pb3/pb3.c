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
\file pb3.c
\brief MAM2 Protobuf3 layer.
*/
#include "mam/v2/pb3/pb3.h"
#include "mam/v2/mss/mss.h"
#include "mam/v2/prng/prng.h"

trits_t pb3_trits_take(trits_t *b, size_t n) { return trits_advance(b, n); }

/*
Protobuf3 primitive types
null, tryte, size_t, trytes, tryte [n]
*/

size_t pb3_sizeof_tryte() { return 3; }

void pb3_encode_tryte(tryte_t t, trits_t *b) {
  MAM2_ASSERT(b && !(trits_size(*b) < pb3_sizeof_tryte()));
  trits_put3(pb3_trits_take(b, 3), t);
}

err_t pb3_decode_tryte(tryte_t *t, trits_t *b) {
  MAM2_ASSERT(0 != t);

  if (trits_size(*b) < pb3_sizeof_tryte()) return err_pb3_eof;

  *t = trits_get3(pb3_trits_take(b, 3));
  return err_ok;
}

size_t pb3_sizeof_trint() { return 9; }

void pb3_encode_trint(trint9_t t, trits_t *b) {
  MAM2_ASSERT(b && !(trits_size(*b) < pb3_sizeof_trint()));
  trits_put9(pb3_trits_take(b, 9), t);
}

err_t pb3_decode_trint(trint9_t *t, trits_t *b) {
  MAM2_ASSERT(0 != t);

  if (trits_size(*b) < pb3_sizeof_trint()) return err_pb3_eof;

  *t = trits_get9(pb3_trits_take(b, 9));
  return err_ok;
}

size_t pb3_sizeof_longtrint() { return 18; }

void pb3_encode_longtrint(trint18_t t, trits_t *b) {
  MAM2_ASSERT(b && !(trits_size(*b) < pb3_sizeof_longtrint()));
  trits_put18(pb3_trits_take(b, 18), t);
}

err_t pb3_decode_longtrint(trint18_t *t, trits_t *b) {
  MAM2_ASSERT(0 != t);

  if (trits_size(*b) < pb3_sizeof_longtrint()) return err_pb3_eof;

  *t = trits_get18(pb3_trits_take(b, 18));
  return err_ok;
}

static size_t pb3_sizet_trytes(size_t n) {
  size_t const max_d = (5 * sizeof(size_t) + 2) / 3;

  size_t d = 0, m = 1;
  for (; d < max_d && (n > (m - 1) / 2);) m *= 27, ++d;

  return d;
}

size_t pb3_sizeof_sizet(size_t n) {
  size_t d = pb3_sizet_trytes(n);
  MAM2_ASSERT(d < 14);

  /* one extra tryte to encode `d` */
  return 3 * (d + 1);
}

void pb3_encode_sizet(size_t n, trits_t *b) {
  size_t d;

  MAM2_ASSERT(b && !(trits_size(*b) < pb3_sizeof_sizet(n)));
  d = pb3_sizet_trytes(n);
  MAM2_ASSERT(d < 14);

  pb3_encode_tryte((tryte_t)d, b);

  for (; d--; n = MAM2_DIVS(n, 27, 27))
    pb3_encode_tryte((tryte_t)MAM2_MODS(n, 27, 27), b);
  MAM2_ASSERT(0 == n);
}

err_t pb3_decode_sizet(size_t *n, trits_t *b) {
  err_t e = err_internal_error;

  MAM2_ASSERT(0 != n);

  do {
    size_t m, s_pos, s_neg;
    tryte_t t;

    tryte_t d;
    err_guard(trits_size(*b) >= 3, err_pb3_eof);
    d = trits_get3(*b);
    *b = trits_drop(*b, 3);
    err_guard(0 <= d && d <= 13, err_invalid_value);

    err_guard(trits_size(*b) >= 3 * (size_t)d, err_pb3_eof);
    for (s_pos = s_neg = 0, m = 1; d--; *b = trits_drop(*b, 3), m *= 27) {
      if ((t = trits_get3(*b)) < 0)
        s_neg += (size_t)(-t) * m;
      else
        s_pos += (size_t)(t)*m;
    }
    err_guard(s_pos >= s_neg, err_negative_value);

    *n = s_pos - s_neg;

    e = err_ok;
  } while (0);

  return e;
}

size_t pb3_sizeof_ntrytes(size_t n) { return 3 * n; }

void pb3_encode_ntrytes(trits_t ntrytes, trits_t *b) {
  size_t n = trits_size(ntrytes);
  MAM2_ASSERT(0 == (n % 3));
  MAM2_ASSERT(b && n <= trits_size(*b));

  trits_copy(ntrytes, pb3_trits_take(b, n));
}

static trits_t pb3_encode_ntrytes2(size_t n, trits_t *b) {
  MAM2_ASSERT(0 == (n % 3));
  MAM2_ASSERT(b && n <= trits_size(*b));

  return pb3_trits_take(b, n);
}

err_t pb3_decode_ntrytes(trits_t ntrytes, trits_t *b) {
  err_t e = err_internal_error;
  size_t n = trits_size(ntrytes);
  MAM2_ASSERT(b && 0 == (n % 3));

  do {
    err_guard(n <= trits_size(*b), err_pb3_eof);
    trits_copy(pb3_trits_take(b, n), ntrytes);

    e = err_ok;
  } while (0);

  return e;
}

static err_t pb3_decode_ntrytes2(trits_t *ntrytes, trits_t *b) {
  err_t e = err_internal_error;
  size_t n;
  MAM2_ASSERT(ntrytes);
  n = trits_size(*ntrytes);
  MAM2_ASSERT(b && 0 == (n % 3));

  do {
    err_guard(n <= trits_size(*b), err_pb3_eof);
    *ntrytes = pb3_trits_take(b, n);

    e = err_ok;
  } while (0);

  return e;
}

#if 0
 size_t pb3_sizeof_trytes(size_t n)
{
  return 0
    + pb3_sizeof_sizet(n)
    + pb3_sizeof_ntrytes(n)
    ;
}

 void pb3_encode_trytes(trits_t trytes, trits_t *b)
{
  size_t n = trits_size(trytes);
  MAM2_ASSERT(0 == (n % 3));

  pb3_encode_sizet(n/3, b);
  pb3_encode_ntrytes(trytes, b);
}

 err_t pb3_decode_trytes(trits_t *trytes, trits_t *b)
{
  err_t e = err_internal_error;

  MAM2_ASSERT(0 != trytes);

  do {
    size_t n = 0;
    err_bind(pb3_decode_sizet(&n, b));
    err_guard(trits_size(*b) >= pb3_sizeof_ntrytes(n), err_pb3_eof);

    err_guard(trits_size(*trytes) >= 3*n, err_buffer_too_small);
    err_bind(pb3_decode_ntrytes(*trytes, b));
    *trytes = trits_drop(*trytes, 3*n);

    e = err_ok;
  } while(0);

  return e;
}

 err_t pb3_decode_trytes2(ialloc *a, trits_t *trytes, trits_t *b)
{
  err_t e = err_internal_error;
  trits_t t;
  t = trits_null();

  MAM2_ASSERT(0 != trytes);

  do {
    size_t n = 0;
    err_bind(pb3_decode_sizet(&n, b));
    err_guard(trits_size(*b) >= pb3_sizeof_ntrytes(n), err_pb3_eof);

    if(trits_is_null(*trytes))
    {
      t = trits_alloc(a, 3 * n);
      err_guard(!trits_is_null(t), err_bad_alloc);
      err_bind(pb3_decode_ntrytes(t, b));
      *trytes = t;
      t = trits_null();
    } else
    {
      err_guard(trits_size(*trytes) >= 3*n, err_buffer_too_small);
      err_bind(pb3_decode_ntrytes(*trytes, b));
      *trytes = trits_drop(*trytes, 3*n);
    }

    e = err_ok;
  } while(0);

  if(!trits_is_null(t))
    trits_free(a, t);

  return e;
}
#endif

/*
Protobuf3 spongos modifier handling
*/

void pb3_wrap_absorb_tryte(ispongos *s, trits_t *b, tryte_t t) {
  trits_t b0 = *b;
  pb3_encode_tryte(t, b);
  spongos_absorb(s, trits_diff(b0, *b));
}
err_t pb3_unwrap_absorb_tryte(ispongos *s, trits_t *b, tryte_t *t) {
  err_t e = err_internal_error;
  trits_t b0;
  do {
    b0 = *b;
    err_bind(pb3_decode_tryte(t, b));
    spongos_absorb(s, trits_diff(b0, *b));
    e = err_ok;
  } while (0);
  return e;
}
void pb3_wrap_absorb_trint(ispongos *s, trits_t *b, trint9_t t) {
  trits_t b0 = *b;
  pb3_encode_trint(t, b);
  spongos_absorb(s, trits_diff(b0, *b));
}
err_t pb3_unwrap_absorb_trint(ispongos *s, trits_t *b, trint9_t *t) {
  err_t e = err_internal_error;
  trits_t b0;
  do {
    b0 = *b;
    err_bind(pb3_decode_trint(t, b));
    spongos_absorb(s, trits_diff(b0, *b));
    e = err_ok;
  } while (0);
  return e;
}
void pb3_wrap_absorb_longtrint(ispongos *s, trits_t *b, trint18_t t) {
  trits_t b0 = *b;
  pb3_encode_longtrint(t, b);
  spongos_absorb(s, trits_diff(b0, *b));
}
err_t pb3_unwrap_absorb_longtrint(ispongos *s, trits_t *b, trint18_t *t) {
  err_t e = err_internal_error;
  trits_t b0;
  do {
    b0 = *b;
    err_bind(pb3_decode_longtrint(t, b));
    spongos_absorb(s, trits_diff(b0, *b));
    e = err_ok;
  } while (0);
  return e;
}
void pb3_wrap_absorb_sizet(ispongos *s, trits_t *b, size_t t) {
  trits_t b0 = *b;
  pb3_encode_sizet(t, b);
  spongos_absorb(s, trits_diff(b0, *b));
}
err_t pb3_unwrap_absorb_sizet(ispongos *s, trits_t *b, size_t *t) {
  err_t e = err_internal_error;
  trits_t b0;
  do {
    b0 = *b;
    err_bind(pb3_decode_sizet(t, b));
    spongos_absorb(s, trits_diff(b0, *b));
    e = err_ok;
  } while (0);
  return e;
}

void pb3_wrap_absorb_ntrytes(ispongos *s, trits_t *b, trits_t t) {
  trits_t b0 = *b;
  pb3_encode_ntrytes(t, b);
  spongos_absorb(s, trits_diff(b0, *b));
}
err_t pb3_unwrap_absorb_ntrytes(ispongos *s, trits_t *b, trits_t t) {
  err_t e = err_internal_error;
  trits_t b0;
  do {
    b0 = *b;
    err_bind(pb3_decode_ntrytes(t, b));
    spongos_absorb(s, trits_diff(b0, *b));
    e = err_ok;
  } while (0);
  return e;
}
void pb3_wrap_crypt_ntrytes(ispongos *s, trits_t *b, trits_t t) {
  size_t n = trits_size(t);
  MAM2_ASSERT(0 == (n % 3));
  MAM2_ASSERT(n <= trits_size(*b));

  spongos_encr(s, t, pb3_trits_take(b, n));
}
err_t pb3_unwrap_crypt_ntrytes(ispongos *s, trits_t *b, trits_t t) {
  err_t e = err_internal_error;
  size_t n = trits_size(t);
  MAM2_ASSERT(0 == (n % 3));

  do {
    err_guard(n <= trits_size(*b), err_pb3_eof);
    spongos_decr(s, pb3_trits_take(b, n), t);

    e = err_ok;
  } while (0);

  return e;
}
void pb3_wrap_squeeze_ntrytes(ispongos *s, trits_t *b, size_t n) {
  MAM2_ASSERT(pb3_sizeof_ntrytes(n) <= trits_size(*b));
  spongos_squeeze(s, pb3_trits_take(b, pb3_sizeof_ntrytes(n)));
}
err_t pb3_unwrap_squeeze_ntrytes(ispongos *s, trits_t *b, size_t n) {
  err_t e = err_internal_error;
  bool_t r;

  do {
    err_guard(pb3_sizeof_ntrytes(n) <= trits_size(*b), err_pb3_eof);
    r = spongos_squeeze_eq(s, pb3_trits_take(b, pb3_sizeof_ntrytes(n)));
    err_guard(r, err_pb3_bad_mac);
    e = err_ok;
  } while (0);

  return e;
}

void pb3_absorb_external_ntrytes(ispongos *s, trits_t t) {
  MAM2_ASSERT(0 == (trits_size(t) % 3));
  spongos_absorb(s, t);
}
void pb3_squeeze_external_ntrytes(ispongos *s, trits_t t) {
  MAM2_ASSERT(0 == (trits_size(t) % 3));
  spongos_squeeze(s, t);
}
