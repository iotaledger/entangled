
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
\file pb3.c
\brief MAM2 Protobuf3 layer.
*/
#include "mam/v2/pb3.h"

#include "mam/v2/mss.h"
#include "mam/v2/prng.h"

/*
Protobuf3 primitive types
null, tryte, size_t, trytes, tryte [n]
*/

MAM2_INLINE MAM2_SAPI size_t pb3_sizeof_tryte() { return 3; }

MAM2_SAPI void pb3_encode_tryte(tryte_t t, trits_t *b) {
  MAM2_ASSERT(b && !(trits_size(*b) < pb3_sizeof_tryte()));
  trits_put3(trits_take(*b, 3), t);
  *b = trits_drop(*b, 3);
}

MAM2_SAPI err_t pb3_decode_tryte(tryte_t *t, trits_t *b) {
  MAM2_ASSERT(0 != t);

  if (trits_size(*b) < pb3_sizeof_tryte()) return err_pb3_eof;

  *t = trits_get3(trits_take(*b, 3));
  *b = trits_drop(*b, 3);
  return err_ok;
}

MAM2_INLINE MAM2_SAPI size_t pb3_sizeof_trint() { return 9; }

MAM2_SAPI void pb3_encode_trint(trint9_t t, trits_t *b) {
  MAM2_ASSERT(b && !(trits_size(*b) < pb3_sizeof_trint()));
  trits_put9(trits_take(*b, 9), t);
  *b = trits_drop(*b, 9);
}

MAM2_SAPI err_t pb3_decode_trint(trint9_t *t, trits_t *b) {
  MAM2_ASSERT(0 != t);

  if (trits_size(*b) < pb3_sizeof_trint()) return err_pb3_eof;

  *t = trits_get9(trits_take(*b, 9));
  *b = trits_drop(*b, 9);
  return err_ok;
}

MAM2_INLINE MAM2_SAPI size_t pb3_sizeof_longtrint() { return 18; }

MAM2_SAPI void pb3_encode_longtrint(trint18_t t, trits_t *b) {
  MAM2_ASSERT(b && !(trits_size(*b) < pb3_sizeof_longtrint()));
  trits_put18(trits_take(*b, 18), t);
  *b = trits_drop(*b, 18);
}

MAM2_SAPI err_t pb3_decode_longtrint(trint18_t *t, trits_t *b) {
  MAM2_ASSERT(0 != t);

  if (trits_size(*b) < pb3_sizeof_longtrint()) return err_pb3_eof;

  *t = trits_get18(trits_take(*b, 18));
  *b = trits_drop(*b, 18);
  return err_ok;
}

static size_t pb3_sizet_trytes(size_t n) {
  size_t const max_d = (5 * sizeof(size_t) + 2) / 3;

  size_t d = 0, m = 1;
  for (; d < max_d && (n > (m - 1) / 2);) m *= 27, ++d;

  return d;
}

MAM2_SAPI size_t pb3_sizeof_sizet(size_t n) {
  size_t d = pb3_sizet_trytes(n);
  MAM2_ASSERT(d < 14);

  // one extra tryte to encode `d`
  return 3 * (d + 1);
}

MAM2_SAPI void pb3_encode_sizet(size_t n, trits_t *b) {
  MAM2_ASSERT(b && !(trits_size(*b) < pb3_sizeof_sizet(n)));

  size_t d = pb3_sizet_trytes(n);
  MAM2_ASSERT(d < 14);

  pb3_encode_tryte((tryte_t)d, b);

  for (; d--; n = MAM2_DIVS(n, 27, 27))
    pb3_encode_tryte((tryte_t)MAM2_MODS(n, 27, 27), b);
  MAM2_ASSERT(0 == n);
}

MAM2_SAPI err_t pb3_decode_sizet(size_t *n, trits_t *b) {
  err_t e = err_internal_error;

  MAM2_ASSERT(0 != n);

  do {
    size_t m, s_pos, s_neg;
    tryte_t t;

    tryte_t d;
    err_guard(trits_size(*b) >= 3, err_pb3_eof);
    d = trits_get3(*b);
    *b = trits_drop(*b, 3);
    err_guard(0 <= d && d < 14, err_invalid_value);

    err_guard(trits_size(*b) >= 3 * d, err_pb3_eof);
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

MAM2_SAPI size_t pb3_sizeof_ntrytes(size_t n) { return 3 * n; }

MAM2_SAPI void pb3_encode_ntrytes(trits_t ntrytes, trits_t *b) {
  size_t n = trits_size(ntrytes);
  MAM2_ASSERT(0 == (n % 3));
  MAM2_ASSERT(n <= trits_size(*b));

  trits_copy(ntrytes, trits_take(*b, n));
  *b = trits_drop(*b, n);
}

MAM2_SAPI err_t pb3_decode_ntrytes(trits_t ntrytes, trits_t *b) {
  err_t e = err_internal_error;

  MAM2_ASSERT(0 == (trits_size(ntrytes) % 3));

  do {
    size_t n = trits_size(ntrytes);
    err_guard(n <= trits_size(*b), err_pb3_eof);
    trits_copy(trits_take(*b, n), ntrytes);
    *b = trits_drop(*b, n);

    e = err_ok;
  } while (0);

  return e;
}

MAM2_SAPI size_t pb3_sizeof_trytes(size_t n) {
  return 0 + pb3_sizeof_sizet(n) + pb3_sizeof_ntrytes(n);
}

MAM2_SAPI void pb3_encode_trytes(trits_t trytes, trits_t *b) {
  size_t n = trits_size(trytes);
  MAM2_ASSERT(0 == (n % 3));

  pb3_encode_sizet(n / 3, b);
  pb3_encode_ntrytes(trytes, b);
}

MAM2_SAPI err_t pb3_decode_trytes(trits_t *trytes, trits_t *b) {
  err_t e = err_internal_error;

  MAM2_ASSERT(0 != trytes);

  do {
    size_t n = 0;
    err_bind(pb3_decode_sizet(&n, b));
    err_guard(trits_size(*b) >= pb3_sizeof_ntrytes(n), err_pb3_eof);

    err_guard(trits_size(*trytes) >= 3 * n, err_buffer_too_small);
    err_bind(pb3_decode_ntrytes(*trytes, b));
    *trytes = trits_drop(*trytes, 3 * n);

    e = err_ok;
  } while (0);

  return e;
}

MAM2_SAPI err_t pb3_decode_trytes2(trits_t *trytes, trits_t *b) {
  err_t e = err_internal_error;
  trits_t t = trits_null();

  MAM2_ASSERT(0 != trytes);

  do {
    size_t n = 0;
    err_bind(pb3_decode_sizet(&n, b));
    err_guard(trits_size(*b) >= pb3_sizeof_ntrytes(n), err_pb3_eof);

    if (trits_is_null(*trytes)) {
      t = trits_alloc(3 * n);
      err_guard(!trits_is_null(t), err_bad_alloc);
      err_bind(pb3_decode_ntrytes(t, b));
      *trytes = t;
      t = trits_null();
    } else {
      err_guard(trits_size(*trytes) >= 3 * n, err_buffer_too_small);
      err_bind(pb3_decode_ntrytes(*trytes, b));
      *trytes = trits_drop(*trytes, 3 * n);
    }

    e = err_ok;
  } while (0);

  if (!trits_is_null(t)) trits_free(t);

  return e;
}

/*
Protobuf3 cryptographic modifier handling:
secret, encrypted, data(other), donthash
*/

MAM2_SAPI void pb3_wrap_secret(isponge *s, trits_t t) {
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, t);
}

MAM2_SAPI void pb3_unwrap_secret(isponge *s, trits_t t) {
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, t);
}

MAM2_SAPI void pb3_wrap_encrypted(isponge *s, trits_t t) {
  sponge_encr(s, t, t);
}

MAM2_SAPI void pb3_unwrap_encrypted(isponge *s, trits_t t) {
  sponge_decr(s, t, t);
}

MAM2_SAPI void pb3_wrap_data(isponge *s, trits_t t) {
  sponge_absorb(s, MAM2_SPONGE_CTL_DATA, t);
}

MAM2_SAPI void pb3_unwrap_data(isponge *s, trits_t t) {
  sponge_absorb(s, MAM2_SPONGE_CTL_DATA, t);
}

#define MAM2_PB3_WRAP(mod, s, b, f)               \
  do {                                            \
    trits_t b##_begin = *b;                       \
    f;                                            \
    pb3_wrap_##mod(s, trits_diff(b##_begin, *b)); \
  } while (0)

#define MAM2_PB3_UNWRAP(mod, s, b, f)               \
  do {                                              \
    trits_t b##_begin = *b;                         \
    e = f;                                          \
    pb3_unwrap_##mod(s, trits_diff(b##_begin, *b)); \
  } while (0)

static bool_t pb3_test_sizet(size_t n) {
  err_t e;
  bool_t ok = 1;
  size_t k = pb3_sizeof_sizet(n);
  size_t m = 0;
  MAM2_TRITS_DEF(b0, 3 * 14);  // 14 trytes max
  trits_t b;

  b = trits_take(b0, k);
  pb3_encode_sizet(n, &b);
  ok = ok && trits_is_empty(b);

  b = trits_take(b0, k);
  e = pb3_decode_sizet(&m, &b);
  ok = ok && trits_is_empty(b);

  return (e == err_ok && ok && m == n);
}

static bool_t pb3_test_sizets() {
  bool_t r = 1;

  size_t n, k;

  n = 0;
  for (k = 0; r && k < 1000; ++k) r = pb3_test_sizet(n++);

  if ((5 * sizeof(size_t) + 2) / 3 < 14) {
    for (n = 1, k = 0; k < sizeof(size_t); ++k) n *= 243;
    n -= 50;
    for (k = 0; r && k < 100; ++k) r = pb3_test_sizet(n++);

    n = (size_t)0 - 100;
    for (k = 0; r && k < 100; ++k) r = pb3_test_sizet(n++);
  }

  n = 2026277576509488133;  //(26^13-1)/2
  n -= 99;
  for (k = 0; r && k < 100; ++k) r = pb3_test_sizet(n++);

  return r;
}

MAM2_SAPI bool_t pb3_test() {
  bool_t r = 1;
  r = r && pb3_test_sizets();
  /*TODO*/
  return r;
}
