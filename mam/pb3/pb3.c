/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/pb3/pb3.h"
#include "common/defs.h"
#include "common/trinary/trit_long.h"

trits_t pb3_trits_take(trits_t *const buffer, size_t const n) { return trits_advance(buffer, n); }

/**
 * Protobuf3 primitive types
 * null, tryte, size_t, trytes, tryte [n]
 */

size_t pb3_sizeof_tryte() { return NUMBER_OF_TRITS_IN_A_TRYTE; }

void pb3_encode_tryte(tryte_t const tryte, trits_t *const buffer) {
  MAM_ASSERT(buffer && !(trits_size(*buffer) < pb3_sizeof_tryte()));

  trits_put3(pb3_trits_take(buffer, NUMBER_OF_TRITS_IN_A_TRYTE), tryte);
}

retcode_t pb3_decode_tryte(tryte_t *const tryte, trits_t *const buffer) {
  MAM_ASSERT(tryte);

  if (trits_size(*buffer) < pb3_sizeof_tryte()) {
    return RC_MAM_PB3_EOF;
  }

  *tryte = trits_get3(pb3_trits_take(buffer, NUMBER_OF_TRITS_IN_A_TRYTE));

  return RC_OK;
}

size_t pb3_sizeof_trint() { return 9; }

void pb3_encode_trint(trint9_t const trint, trits_t *const buffer) {
  MAM_ASSERT(buffer && !(trits_size(*buffer) < pb3_sizeof_trint()));

  trits_put9(pb3_trits_take(buffer, 9), trint);
}

retcode_t pb3_decode_trint(trint9_t *const trint, trits_t *const buffer) {
  MAM_ASSERT(trint);

  if (trits_size(*buffer) < pb3_sizeof_trint()) {
    return RC_MAM_PB3_EOF;
  }

  *trint = trits_get9(pb3_trits_take(buffer, 9));

  return RC_OK;
}

size_t pb3_sizeof_longtrint() { return 18; }

void pb3_encode_longtrint(trint18_t const trint, trits_t *const buffer) {
  MAM_ASSERT(buffer && !(trits_size(*buffer) < pb3_sizeof_longtrint()));

  trits_put18(pb3_trits_take(buffer, 18), trint);
}

retcode_t pb3_decode_longtrint(trint18_t *const trint, trits_t *const buffer) {
  MAM_ASSERT(trint);

  if (trits_size(*buffer) < pb3_sizeof_longtrint()) {
    return RC_MAM_PB3_EOF;
  }

  *trint = trits_get18(pb3_trits_take(buffer, 18));

  return RC_OK;
}

size_t pb3_sizeof_size_t(size_t const n) { return trits_sizeof_size_t(n); }

void pb3_encode_size_t(size_t size, trits_t *const buffer) { trits_encode_size_t(size, buffer); }

retcode_t pb3_decode_size_t(size_t *const size, trits_t *const buffer) { return trits_decode_size_t(size, buffer); }

size_t pb3_sizeof_ntrytes(size_t const n) { return NUMBER_OF_TRITS_IN_A_TRYTE * n; }

void pb3_encode_ntrytes(trits_t const ntrytes, trits_t *const buffer) {
  size_t n = trits_size(ntrytes);
  MAM_ASSERT(0 == (n % NUMBER_OF_TRITS_IN_A_TRYTE));
  MAM_ASSERT(buffer && n <= trits_size(*buffer));

  trits_copy(ntrytes, pb3_trits_take(buffer, n));
}

retcode_t pb3_decode_ntrytes(trits_t const ntrytes, trits_t *const buffer) {
  size_t n = trits_size(ntrytes);
  MAM_ASSERT(buffer && (n % NUMBER_OF_TRITS_IN_A_TRYTE) == 0);

  if (n > trits_size(*buffer)) {
    return RC_MAM_PB3_EOF;
  }

  trits_copy(pb3_trits_take(buffer, n), ntrytes);

  return RC_OK;
}

/**
 * Protobuf3 spongos modifier handling
 */

void pb3_wrap_absorb_tryte(mam_spongos_t *const spongos, trits_t *const buffer, tryte_t const tryte) {
  trits_t b0 = *buffer;

  pb3_encode_tryte(tryte, buffer);
  mam_spongos_absorb(spongos, trits_diff(b0, *buffer));
}

retcode_t pb3_unwrap_absorb_tryte(mam_spongos_t *const spongos, trits_t *const buffer, tryte_t *const tryte) {
  retcode_t ret = RC_OK;
  trits_t b0;

  b0 = *buffer;
  if ((ret = pb3_decode_tryte(tryte, buffer)) != RC_OK) {
    return ret;
  }
  mam_spongos_absorb(spongos, trits_diff(b0, *buffer));

  return ret;
}

void pb3_wrap_absorb_trint(mam_spongos_t *const spongos, trits_t *const buffer, trint9_t const trint) {
  trits_t b0 = *buffer;

  pb3_encode_trint(trint, buffer);
  mam_spongos_absorb(spongos, trits_diff(b0, *buffer));
}

retcode_t pb3_unwrap_absorb_trint(mam_spongos_t *const spongos, trits_t *const buffer, trint9_t *const trint) {
  retcode_t ret = RC_OK;
  trits_t b0;

  b0 = *buffer;
  if ((ret = pb3_decode_trint(trint, buffer)) != RC_OK) {
    return ret;
  }
  mam_spongos_absorb(spongos, trits_diff(b0, *buffer));

  return ret;
}

void pb3_wrap_absorb_size_t(mam_spongos_t *const spongos, trits_t *const buffer, size_t const size) {
  trits_t b0 = *buffer;

  pb3_encode_size_t(size, buffer);
  mam_spongos_absorb(spongos, trits_diff(b0, *buffer));
}

retcode_t pb3_unwrap_absorb_size_t(mam_spongos_t *const spongos, trits_t *const buffer, size_t *const size) {
  retcode_t ret = RC_OK;
  trits_t b0;

  b0 = *buffer;
  if ((ret = pb3_decode_size_t(size, buffer)) != RC_OK) {
    return ret;
  }
  mam_spongos_absorb(spongos, trits_diff(b0, *buffer));

  return ret;
}

void pb3_wrap_absorb_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, trits_t const trits) {
  trits_t b0 = *buffer;

  pb3_encode_ntrytes(trits, buffer);
  mam_spongos_absorb(spongos, trits_diff(b0, *buffer));
}

retcode_t pb3_unwrap_absorb_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, trits_t const trits) {
  retcode_t ret = RC_OK;
  trits_t b0;

  b0 = *buffer;
  if ((ret = pb3_decode_ntrytes(trits, buffer)) != RC_OK) {
    return ret;
  }
  mam_spongos_absorb(spongos, trits_diff(b0, *buffer));

  return ret;
}

void pb3_wrap_crypt_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, trits_t const trits) {
  size_t n = trits_size(trits);
  MAM_ASSERT(0 == (n % NUMBER_OF_TRITS_IN_A_TRYTE));
  MAM_ASSERT(n <= trits_size(*buffer));

  mam_spongos_encr(spongos, trits, pb3_trits_take(buffer, n));
}

retcode_t pb3_unwrap_crypt_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, trits_t const trits) {
  size_t n = trits_size(trits);
  MAM_ASSERT(0 == (n % NUMBER_OF_TRITS_IN_A_TRYTE));

  if (n > trits_size(*buffer)) {
    return RC_MAM_PB3_EOF;
  }
  mam_spongos_decr(spongos, pb3_trits_take(buffer, n), trits);

  return RC_OK;
}

void pb3_wrap_squeeze_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, size_t const n) {
  MAM_ASSERT(pb3_sizeof_ntrytes(n) <= trits_size(*buffer));

  mam_spongos_squeeze(spongos, pb3_trits_take(buffer, pb3_sizeof_ntrytes(n)));
}

retcode_t pb3_unwrap_squeeze_ntrytes(mam_spongos_t *const spongos, trits_t *const buffer, size_t const n) {
  if (pb3_sizeof_ntrytes(n) > trits_size(*buffer)) {
    return RC_MAM_PB3_EOF;
  }
  if (!mam_spongos_squeeze_eq(spongos, pb3_trits_take(buffer, pb3_sizeof_ntrytes(n)))) {
    return RC_MAM_PB3_BAD_MAC;
  }

  return RC_OK;
}

void pb3_absorb_external_ntrytes(mam_spongos_t *const spongos, trits_t const trits) {
  MAM_ASSERT(0 == (trits_size(trits) % 3));

  mam_spongos_absorb(spongos, trits);
}

void pb3_squeeze_external_ntrytes(mam_spongos_t *const spongos, trits_t const trits) {
  MAM_ASSERT(0 == (trits_size(trits) % 3));

  mam_spongos_squeeze(spongos, trits);
}
