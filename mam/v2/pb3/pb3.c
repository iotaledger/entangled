/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/pb3/pb3.h"

trits_t pb3_trits_take(trits_t *const b, size_t const n) {
  return trits_advance(b, n);
}

/**
 * Protobuf3 primitive types
 * null, tryte, size_t, trytes, tryte [n]
 */

size_t pb3_sizeof_tryte() { return 3; }

void pb3_encode_tryte(tryte_t const tryte, trits_t *const buffer) {
  MAM2_ASSERT(buffer && !(trits_size(*buffer) < pb3_sizeof_tryte()));

  trits_put3(pb3_trits_take(buffer, 3), tryte);
}

retcode_t pb3_decode_tryte(tryte_t *const tryte, trits_t *const buffer) {
  MAM2_ASSERT(tryte);

  if (trits_size(*buffer) < pb3_sizeof_tryte()) {
    return RC_MAM2_PB3_EOF;
  }

  *tryte = trits_get3(pb3_trits_take(buffer, 3));

  return RC_OK;
}

size_t pb3_sizeof_trint() { return 9; }

void pb3_encode_trint(trint9_t const trint, trits_t *const buffer) {
  MAM2_ASSERT(buffer && !(trits_size(*buffer) < pb3_sizeof_trint()));

  trits_put9(pb3_trits_take(buffer, 9), trint);
}

retcode_t pb3_decode_trint(trint9_t *const trint, trits_t *const buffer) {
  MAM2_ASSERT(trint);

  if (trits_size(*buffer) < pb3_sizeof_trint()) {
    return RC_MAM2_PB3_EOF;
  }

  *trint = trits_get9(pb3_trits_take(buffer, 9));

  return RC_OK;
}

size_t pb3_sizeof_longtrint() { return 18; }

void pb3_encode_longtrint(trint18_t const trint, trits_t *const buffer) {
  MAM2_ASSERT(buffer && !(trits_size(*buffer) < pb3_sizeof_longtrint()));

  trits_put18(pb3_trits_take(buffer, 18), trint);
}

retcode_t pb3_decode_longtrint(trint18_t *const trint, trits_t *const buffer) {
  MAM2_ASSERT(trint);

  if (trits_size(*buffer) < pb3_sizeof_longtrint()) {
    return RC_MAM2_PB3_EOF;
  }

  *trint = trits_get18(pb3_trits_take(buffer, 18));

  return RC_OK;
}

static size_t pb3_size_t_trytes(size_t const n) {
  MAM2_ASSERT(n <= MAM2_PB3_SIZE_MAX);

  size_t const max_d = (5 * sizeof(size_t) + 2) / 3;
  size_t d = 0, m = 1;

  for (; d < max_d && (n > (m - 1) / 2);) {
    m *= 27;
    ++d;
  }

  return d;
}

size_t pb3_sizeof_size_t(size_t const n) {
  size_t d = pb3_size_t_trytes(n);
  MAM2_ASSERT(d < 14);

  /* one extra tryte to encode `d` */
  return 3 * (d + 1);
}

void pb3_encode_size_t(size_t n, trits_t *const buffer) {
  MAM2_ASSERT(buffer && !(trits_size(*buffer) < pb3_sizeof_size_t(n)));

  size_t d = pb3_size_t_trytes(n);
  MAM2_ASSERT(d < 14);

  pb3_encode_tryte((tryte_t)d, buffer);

  if (n > 27) {
    /* explicitly unroll the first iteration safely */
    --d;
    pb3_encode_tryte((tryte_t)MAM2_MODS(n - 27, 27, 27), buffer);
    n = 1 + MAM2_DIVS(n - 27, 27, 27);
  }

  for (; d--; n = MAM2_DIVS(n, 27, 27)) {
    pb3_encode_tryte((tryte_t)MAM2_MODS(n, 27, 27), buffer);
  }

  MAM2_ASSERT(0 == n);
}

retcode_t pb3_decode_size_t(size_t *const n, trits_t *const buffer) {
  MAM2_ASSERT(n != 0);

  tryte_t d;

  if (trits_size(*buffer) < 3) {
    return RC_MAM2_PB3_EOF;
  }

  d = trits_get3(*buffer);
  *buffer = trits_drop(*buffer, 3);
  if (d < 0 || d > 13) {
    return RC_MAM2_INVALID_VALUE;
  }
  if (trits_size(*buffer) < 3 * (size_t)d) {
    return RC_MAM2_PB3_EOF;
  }

  /* move pointer to the end */
  *buffer = trits_drop(*buffer, 3 * (size_t)d);

  *n = 0;
  if (0 < d) {
    tryte_t t;
    trits_t s = *buffer;
    uint64_t m;

    --d;
    s = trits_pickup(s, 3);
    /* higher tryte in the representation */
    t = trits_get3(s);
    /* can't be 0 or negative */
    if (t <= 0) {
      return RC_MAM2_INVALID_VALUE;
    }
    m = (size_t)t;

    for (; d--;) {
      s = trits_pickup(s, 3);
      t = trits_get3(s);
      m *= 27;
      m += (uint64_t)t;
    }
    /* value may be truncated here */
    *n = (size_t)m;
    if (m != (uint64_t)*n) {
      return RC_MAM2_PB3_SIZE_T_NOT_SUPPORTED;
    }
  }

  return RC_OK;
}

size_t pb3_sizeof_ntrytes(size_t const n) { return 3 * n; }

void pb3_encode_ntrytes(trits_t const ntrytes, trits_t *const buffer) {
  size_t n = trits_size(ntrytes);
  MAM2_ASSERT(0 == (n % 3));
  MAM2_ASSERT(buffer && n <= trits_size(*buffer));

  trits_copy(ntrytes, pb3_trits_take(buffer, n));
}

retcode_t pb3_decode_ntrytes(trits_t const ntrytes, trits_t *const buffer) {
  size_t n = trits_size(ntrytes);
  MAM2_ASSERT(buffer && (n % 3) == 0);

  if (n > trits_size(*buffer)) {
    return RC_MAM2_PB3_EOF;
  }

  trits_copy(pb3_trits_take(buffer, n), ntrytes);

  return RC_OK;
}

/**
 * Protobuf3 spongos modifier handling
 */

void pb3_wrap_absorb_tryte(spongos_t *const spongos, trits_t *const buffer,
                           tryte_t const tryte) {
  trits_t b0 = *buffer;

  pb3_encode_tryte(tryte, buffer);
  spongos_absorb(spongos, trits_diff(b0, *buffer));
}

retcode_t pb3_unwrap_absorb_tryte(spongos_t *const spongos,
                                  trits_t *const buffer, tryte_t *const tryte) {
  retcode_t ret = RC_OK;
  trits_t b0;

  b0 = *buffer;
  if ((ret = pb3_decode_tryte(tryte, buffer)) != RC_OK) {
    return ret;
  }
  spongos_absorb(spongos, trits_diff(b0, *buffer));

  return ret;
}

void pb3_wrap_absorb_trint(spongos_t *const spongos, trits_t *const buffer,
                           trint9_t const trint) {
  trits_t b0 = *buffer;

  pb3_encode_trint(trint, buffer);
  spongos_absorb(spongos, trits_diff(b0, *buffer));
}

retcode_t pb3_unwrap_absorb_trint(spongos_t *const spongos,
                                  trits_t *const buffer,
                                  trint9_t *const trint) {
  retcode_t ret = RC_OK;
  trits_t b0;

  b0 = *buffer;
  if ((ret = pb3_decode_trint(trint, buffer)) != RC_OK) {
    return ret;
  }
  spongos_absorb(spongos, trits_diff(b0, *buffer));

  return ret;
}

void pb3_wrap_absorb_longtrint(spongos_t *const spongos, trits_t *const buffer,
                               trint18_t const trint) {
  trits_t b0 = *buffer;

  pb3_encode_longtrint(trint, buffer);
  spongos_absorb(spongos, trits_diff(b0, *buffer));
}

retcode_t pb3_unwrap_absorb_longtrint(spongos_t *const spongos,
                                      trits_t *const buffer,
                                      trint18_t *const trint) {
  retcode_t ret = RC_OK;
  trits_t b0;

  b0 = *buffer;
  if ((ret = pb3_decode_longtrint(trint, buffer)) != RC_OK) {
    return ret;
  }
  spongos_absorb(spongos, trits_diff(b0, *buffer));

  return ret;
}

void pb3_wrap_absorb_size_t(spongos_t *const spongos, trits_t *const buffer,
                            size_t const t) {
  trits_t b0 = *buffer;

  pb3_encode_size_t(t, buffer);
  spongos_absorb(spongos, trits_diff(b0, *buffer));
}

retcode_t pb3_unwrap_absorb_size_t(spongos_t *const spongos,
                                   trits_t *const buffer, size_t *const t) {
  retcode_t ret = RC_OK;
  trits_t b0;

  b0 = *buffer;
  if ((ret = pb3_decode_size_t(t, buffer)) != RC_OK) {
    return RC_OK;
  }
  spongos_absorb(spongos, trits_diff(b0, *buffer));

  return ret;
}

void pb3_wrap_absorb_ntrytes(spongos_t *const spongos, trits_t *const buffer,
                             trits_t const trits) {
  trits_t b0 = *buffer;

  pb3_encode_ntrytes(trits, buffer);
  spongos_absorb(spongos, trits_diff(b0, *buffer));
}

retcode_t pb3_unwrap_absorb_ntrytes(spongos_t *const spongos,
                                    trits_t *const buffer,
                                    trits_t const trits) {
  retcode_t ret = RC_OK;
  trits_t b0;

  b0 = *buffer;
  if ((ret = pb3_decode_ntrytes(trits, buffer)) != RC_OK) {
    return ret;
  }
  spongos_absorb(spongos, trits_diff(b0, *buffer));

  return ret;
}

void pb3_wrap_crypt_ntrytes(spongos_t *const spongos, trits_t *const buffer,
                            trits_t const trits) {
  size_t n = trits_size(trits);
  MAM2_ASSERT(0 == (n % 3));
  MAM2_ASSERT(n <= trits_size(*buffer));

  spongos_encr(spongos, trits, pb3_trits_take(buffer, n));
}

retcode_t pb3_unwrap_crypt_ntrytes(spongos_t *const spongos,
                                   trits_t *const buffer, trits_t const trits) {
  size_t n = trits_size(trits);
  MAM2_ASSERT(0 == (n % 3));

  if (n > trits_size(*buffer)) {
    return RC_MAM2_PB3_EOF;
  }
  spongos_decr(spongos, pb3_trits_take(buffer, n), trits);

  return RC_OK;
}

void pb3_wrap_squeeze_ntrytes(spongos_t *const spongos, trits_t *const buffer,
                              size_t const n) {
  MAM2_ASSERT(pb3_sizeof_ntrytes(n) <= trits_size(*buffer));

  spongos_squeeze(spongos, pb3_trits_take(buffer, pb3_sizeof_ntrytes(n)));
}

retcode_t pb3_unwrap_squeeze_ntrytes(spongos_t *const spongos,
                                     trits_t *const buffer, size_t const n) {
  if (pb3_sizeof_ntrytes(n) > trits_size(*buffer)) {
    return RC_MAM2_PB3_EOF;
  }
  if (!spongos_squeeze_eq(spongos,
                          pb3_trits_take(buffer, pb3_sizeof_ntrytes(n)))) {
    return RC_MAM2_PB3_BAD_MAC;
  }

  return RC_OK;
}

void pb3_absorb_external_ntrytes(spongos_t *const spongos,
                                 trits_t const trits) {
  MAM2_ASSERT(0 == (trits_size(trits) % 3));

  spongos_absorb(spongos, trits);
}

void pb3_squeeze_external_ntrytes(spongos_t *const spongos,
                                  trits_t const trits) {
  MAM2_ASSERT(0 == (trits_size(trits) % 3));

  spongos_squeeze(spongos, trits);
}
