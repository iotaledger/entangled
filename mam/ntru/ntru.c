/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include "mam/ntru/ntru.h"
#include "mam/pb3/pb3.h"
#include "utils/memset_safe.h"

static retcode_t ntru_pk_encr_r(mam_ntru_pk_t const *const ntru_pk, mam_spongos_t *const spongos, trits_t const r,
                                trits_t const session_key, trits_t encrypted_session_key);

/*
 * NTRU public key
 */

retcode_t ntru_pk_encr(mam_ntru_pk_t const *const ntru_pk, mam_prng_t const *const prng, mam_spongos_t *const spongos,
                       trits_t const nonce, trits_t const session_key, trits_t encrypted_session_key) {
  trits_t r;

  if (ntru_pk == NULL || prng == NULL || spongos == NULL) {
    return RC_NULL_PARAM;
  }

  MAM_ASSERT(trits_size(session_key) == MAM_NTRU_KEY_SIZE);
  MAM_ASSERT(trits_size(encrypted_session_key) == MAM_NTRU_EKEY_SIZE);
  MAM_ASSERT(!trits_is_same(session_key, encrypted_session_key));

  r = trits_take(encrypted_session_key, MAM_NTRU_SK_SIZE);
  mam_prng_gen3(prng, MAM_PRNG_DST_NTRU_KEY, mam_ntru_pk_id(ntru_pk), session_key, nonce, r);

  return ntru_pk_encr_r(ntru_pk, spongos, r, session_key, encrypted_session_key);
}

retcode_t ntru_pk_encr_r(mam_ntru_pk_t const *const ntru_pk, mam_spongos_t *const spongos, trits_t const r,
                         trits_t const session_key, trits_t encrypted_session_key) {
  MAM_ASSERT(trits_size(r) == MAM_NTRU_SK_SIZE);
  MAM_ASSERT(trits_size(session_key) == MAM_NTRU_KEY_SIZE);
  MAM_ASSERT(trits_size(encrypted_session_key) == MAM_NTRU_EKEY_SIZE);

  MAM_POLY_DEF(h);
  MAM_POLY_DEF(s);

  if (ntru_pk == NULL || spongos == NULL) {
    return RC_NULL_PARAM;
  }

  if (!poly_from_trits(h, mam_ntru_pk_key(ntru_pk))) {
    return RC_MAM_NTRU_POLY_FAILED;
  }
  poly_ntt(h, h);

  /* s(x) := r(x)*h(x) */
  poly_small_from_trits(s, r);
  poly_ntt(s, s);
  poly_conv(s, h, s);
  poly_intt(s, s);

  /* h(x) = AE(r*h;K) */
  poly_to_trits(s, encrypted_session_key);
  mam_spongos_init(spongos);
  mam_spongos_absorb(spongos, encrypted_session_key);
  mam_spongos_commit(spongos);
  mam_spongos_encr(spongos, session_key, trits_take(r, MAM_NTRU_KEY_SIZE));
  mam_spongos_squeeze(spongos, trits_drop(r, MAM_NTRU_KEY_SIZE));
  poly_small_from_trits(h, r);

  /* Y = r*h + AE(r*h;K) */
  poly_add(s, h, s);
  poly_to_trits(s, encrypted_session_key);

  memset_safe(h, sizeof(h), 0, sizeof(h));

  return RC_OK;
}

size_t mam_ntru_pks_serialized_size(mam_ntru_pk_t_set_t const ntru_pks) {
  return pb3_sizeof_size_t(mam_ntru_pk_t_set_size(ntru_pks)) + mam_ntru_pk_t_set_size(ntru_pks) * sizeof(mam_ntru_pk_t);
}

retcode_t mam_ntru_pks_serialize(mam_ntru_pk_t_set_t const ntru_pks, trits_t *const trits) {
  mam_ntru_pk_t_set_entry_t *entry = NULL;
  mam_ntru_pk_t_set_entry_t *tmp = NULL;

  if (trits == NULL) {
    return RC_NULL_PARAM;
  }

  pb3_encode_size_t(mam_ntru_pk_t_set_size(ntru_pks), trits);

  SET_ITER(ntru_pks, entry, tmp) { pb3_encode_ntrytes(trits_from_rep(MAM_NTRU_PK_SIZE, entry->value.key), trits); }

  return RC_OK;
}

retcode_t mam_ntru_pks_deserialize(trits_t *const trits, mam_ntru_pk_t_set_t *const ntru_pks) {
  retcode_t ret = RC_OK;
  size_t ntru_pks_size = 0;
  mam_ntru_pk_t ntru_pk;

  if (trits == NULL || ntru_pks == NULL) {
    return RC_NULL_PARAM;
  }

  ERR_BIND_RETURN(pb3_decode_size_t(&ntru_pks_size, trits), ret);

  for (size_t i = 0; i < ntru_pks_size; i++) {
    ERR_BIND_RETURN(pb3_decode_ntrytes(trits_from_rep(MAM_NTRU_PK_SIZE, ntru_pk.key), trits), ret);
    ERR_BIND_RETURN(mam_ntru_pk_t_set_add(ntru_pks, &ntru_pk), ret);
  }

  return ret;
}

retcode_t ntru_sk_reset(mam_ntru_sk_t *const ntru_sk) {
  if (ntru_sk == NULL) {
    return RC_NULL_PARAM;
  }

  memset_safe(ntru_sk, sizeof(mam_ntru_sk_t), 0, sizeof(mam_ntru_sk_t));

  return RC_OK;
}

void ntru_sk_gen(mam_ntru_sk_t const *const ntru_sk, mam_prng_t const *const prng, trits_t const nonce) {
  MAM_TRITS_DEF(nonce_i, 81);
  MAM_TRITS_DEF(secret_key, 2 * MAM_NTRU_SK_SIZE);
  nonce_i = MAM_TRITS_INIT(nonce_i, 81);
  secret_key = MAM_TRITS_INIT(secret_key, 2 * MAM_NTRU_SK_SIZE);
  MAM_POLY_DEF(g);
  MAM_POLY_DEF(h);
  poly_coeff_t *f = NULL;

  if (ntru_sk == NULL || prng == NULL) {
    return;
  }

  f = (poly_coeff_t *)ntru_sk->f;

  trits_set_zero(nonce_i);
  do {
    mam_prng_gen2(prng, MAM_PRNG_DST_NTRU_KEY, nonce, nonce_i, secret_key);
    poly_small_from_trits(f, trits_take(secret_key, MAM_NTRU_SK_SIZE));
    poly_small_from_trits(g, trits_drop(secret_key, MAM_NTRU_SK_SIZE));

    /* f := NTT(1+3f) */
    poly_small_mul3(f, f);
    poly_small3_add1(f);
    poly_ntt(f, f);

    /* g := NTT(g) */
    poly_ntt(g, g);
  } while (!(poly_has_inv(f) && poly_has_inv(g)) && trits_inc(nonce_i));

  trits_copy(trits_take(secret_key, MAM_NTRU_SK_SIZE), ntru_sk_key(ntru_sk));

  /* h := 3g/(1+3f) */
  poly_small_mul3(g, g);
  poly_inv(f, h);
  poly_conv(g, h, h);
  poly_intt(h, h);

  poly_to_trits(h, ntru_sk_pk_key(ntru_sk));

  memset_safe(trits_begin(nonce_i), trits_size(nonce_i), 0, trits_size(nonce_i));
  memset_safe(trits_begin(secret_key), trits_size(secret_key), 0, trits_size(secret_key));
}

bool ntru_sk_decr(mam_ntru_sk_t const *const ntru_sk, mam_spongos_t *const spongos, trits_t const encrypted_session_key,
                  trits_t session_key) {
  bool eq = false;
  poly_coeff_t *f = NULL;
  MAM_POLY_DEF(s);
  MAM_POLY_DEF(r);
  MAM_TRITS_DEF(kt, MAM_NTRU_SK_SIZE);
  MAM_TRITS_DEF(rh, MAM_NTRU_EKEY_SIZE);
  MAM_TRITS_DEF(m, MAM_NTRU_SK_SIZE - MAM_NTRU_KEY_SIZE);
  kt = MAM_TRITS_INIT(kt, MAM_NTRU_SK_SIZE);
  rh = MAM_TRITS_INIT(rh, MAM_NTRU_EKEY_SIZE);
  m = MAM_TRITS_INIT(m, MAM_NTRU_SK_SIZE - MAM_NTRU_KEY_SIZE);

  if (ntru_sk == NULL || spongos == NULL) {
    return false;
  }

  MAM_ASSERT(trits_size(session_key) == MAM_NTRU_KEY_SIZE);
  MAM_ASSERT(trits_size(encrypted_session_key) == MAM_NTRU_EKEY_SIZE);

  /* f is NTT form */
  f = (poly_coeff_t *)ntru_sk->f;

  /* t(x) := Y */
  if (!poly_from_trits(s, encrypted_session_key)) {
    return false;
  }

  /* r(x) := s(x)*(1+3f(x)) (mods 3) */
  poly_ntt(s, r);
  poly_conv(r, f, r);
  poly_intt(r, r);
  poly_round_to_trits(r, kt);
  poly_small_from_trits(r, kt);

  /* s(x) := Y - r(x) */
  poly_sub(s, r, s);
  poly_to_trits(s, rh);

  /* K = AD(rh;kt) */
  mam_spongos_init(spongos);
  mam_spongos_absorb(spongos, rh);
  mam_spongos_commit(spongos);
  mam_spongos_decr(spongos, trits_take(kt, MAM_NTRU_KEY_SIZE), session_key);
  mam_spongos_squeeze(spongos, m);
  eq = trits_cmp_eq(m, trits_drop(kt, MAM_NTRU_KEY_SIZE));

  memset_safe(trits_begin(kt), trits_size(kt), 0, trits_size(kt));
  memset_safe(trits_begin(rh), trits_size(rh), 0, trits_size(rh));
  memset_safe(trits_begin(m), trits_size(m), 0, trits_size(m));
  memset_safe(s, sizeof(s), 0, sizeof(s));
  memset_safe(r, sizeof(r), 0, sizeof(r));

  return eq;
}

void ntru_sk_load(mam_ntru_sk_t *const ntru_sk) {
  poly_coeff_t *f = NULL;

  if (ntru_sk == NULL) {
    return;
  }

  f = (poly_coeff_t *)ntru_sk->f;

  poly_small_from_trits(f, ntru_sk_key(ntru_sk));
  /* f := NTT(1+3f) */
  poly_small_mul3(f, f);
  poly_small3_add1(f);
  poly_ntt(f, f);
}

void mam_ntru_sks_destroy(mam_ntru_sk_t_set_t *const ntru_sks) {
  mam_ntru_sk_t_set_entry_t *entry = NULL;
  mam_ntru_sk_t_set_entry_t *tmp = NULL;

  if (ntru_sks == NULL || *ntru_sks == NULL) {
    return;
  }

  SET_ITER(*ntru_sks, entry, tmp) {
    ntru_sk_reset(&entry->value);
    mam_ntru_sk_t_set_remove_entry(ntru_sks, entry);
  }
}

size_t mam_ntru_sks_serialized_size(mam_ntru_sk_t_set_t const ntru_sks) {
  return pb3_sizeof_size_t(mam_ntru_sk_t_set_size(ntru_sks)) +
         mam_ntru_sk_t_set_size(ntru_sks) * (MAM_NTRU_PK_SIZE + MAM_NTRU_SK_SIZE);
}

retcode_t mam_ntru_sks_serialize(mam_ntru_sk_t_set_t const ntru_sks, trits_t *const trits) {
  mam_ntru_sk_t_set_entry_t *entry = NULL;
  mam_ntru_sk_t_set_entry_t *tmp = NULL;

  if (trits == NULL) {
    return RC_NULL_PARAM;
  }

  pb3_encode_size_t(mam_ntru_sk_t_set_size(ntru_sks), trits);

  SET_ITER(ntru_sks, entry, tmp) {
    pb3_encode_ntrytes(trits_from_rep(MAM_NTRU_PK_SIZE, entry->value.public_key.key), trits);
    trits_copy(trits_from_rep(MAM_NTRU_SK_SIZE, entry->value.secret_key), trits_take(*trits, MAM_NTRU_SK_SIZE));
    trits_advance(trits, MAM_NTRU_SK_SIZE);
  }

  return RC_OK;
}

retcode_t mam_ntru_sks_deserialize(trits_t *const trits, mam_ntru_sk_t_set_t *const ntru_sks) {
  retcode_t ret = RC_OK;
  size_t ntru_sks_size = 0;
  mam_ntru_sk_t ntru_sk;

  if (trits == NULL || ntru_sks == NULL) {
    return RC_NULL_PARAM;
  }

  ntru_sk_reset(&ntru_sk);

  ERR_BIND_RETURN(pb3_decode_size_t(&ntru_sks_size, trits), ret);

  for (size_t i = 0; i < ntru_sks_size; i++) {
    ERR_BIND_RETURN(pb3_decode_ntrytes(trits_from_rep(MAM_NTRU_PK_SIZE, ntru_sk.public_key.key), trits), ret);
    trits_copy(trits_take(*trits, MAM_NTRU_SK_SIZE), trits_from_rep(MAM_NTRU_SK_SIZE, ntru_sk.secret_key));
    trits_advance(trits, MAM_NTRU_SK_SIZE);
    ntru_sk_load(&ntru_sk);
    ERR_BIND_RETURN(mam_ntru_sk_t_set_add(ntru_sks, &ntru_sk), ret);
  }

  ntru_sk_reset(&ntru_sk);

  return ret;
}
