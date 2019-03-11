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

trits_t mam_ntru_pk_id(mam_ntru_pk_t const *const ntru_pk) {
  return trits_from_rep(MAM_NTRU_ID_SIZE, ntru_pk->key);
}

trits_t mam_ntru_pk_key(mam_ntru_pk_t const *const ntru_pk) {
  return trits_from_rep(MAM_NTRU_PK_SIZE, ntru_pk->key);
}

retcode_t ntru_init(mam_ntru_sk_t *const ntru) {
  MAM_ASSERT(ntru);

  memset_safe(ntru, sizeof(mam_ntru_sk_t), 0, sizeof(mam_ntru_sk_t));

  return RC_OK;
}

retcode_t ntru_destroy(mam_ntru_sk_t *const ntru) {
  MAM_ASSERT(ntru);

  memset_safe(ntru, sizeof(mam_ntru_sk_t), 0, sizeof(mam_ntru_sk_t));

  return RC_OK;
}

void ntru_gen(mam_ntru_sk_t const *const ntru, mam_prng_t const *const prng,
              trits_t const nonce) {
  MAM_TRITS_DEF0(nonce_i, 81);
  MAM_TRITS_DEF0(secret_key, 2 * MAM_NTRU_SK_SIZE);
  nonce_i = MAM_TRITS_INIT(nonce_i, 81);
  secret_key = MAM_TRITS_INIT(secret_key, 2 * MAM_NTRU_SK_SIZE);
  MAM_POLY_DEF(g);
  MAM_POLY_DEF(h);
  poly_coeff_t *f = (poly_coeff_t *)ntru->f;

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

  trits_copy(trits_take(secret_key, MAM_NTRU_SK_SIZE), ntru_sk_trits(ntru));

  /* h := 3g/(1+3f) */
  poly_small_mul3(g, g);
  poly_inv(f, h);
  poly_conv(g, h, h);
  poly_intt(h, h);

  poly_to_trits(h, ntru_pk_trits(ntru));

  memset_safe(trits_begin(nonce_i), trits_size(nonce_i), 0,
              trits_size(nonce_i));
  memset_safe(trits_begin(secret_key), trits_size(secret_key), 0,
              trits_size(secret_key));
}

void ntru_encr(trits_t const public_key, mam_prng_t const *const prng,
               mam_spongos_t *const spongos, trits_t const session_key,
               trits_t const nonce, trits_t encrypted_session_key) {
  MAM_ASSERT(trits_size(session_key) == MAM_NTRU_KEY_SIZE);
  MAM_ASSERT(trits_size(encrypted_session_key) == MAM_NTRU_EKEY_SIZE);
  MAM_ASSERT(!trits_is_same(session_key, encrypted_session_key));

  trits_t r;

  r = trits_take(encrypted_session_key, MAM_NTRU_SK_SIZE);
  mam_prng_gen3(prng, MAM_PRNG_DST_NTRU_KEY,
                trits_take(public_key, MAM_NTRU_ID_SIZE), session_key, nonce,
                r);
  ntru_encr_r(public_key, spongos, r, session_key, encrypted_session_key);
}

void ntru_encr_r(trits_t const public_key, mam_spongos_t *const spongos,
                 trits_t const r, trits_t const session_key,
                 trits_t encrypted_session_key) {
  MAM_ASSERT(trits_size(r) == MAM_NTRU_SK_SIZE);
  MAM_ASSERT(trits_size(session_key) == MAM_NTRU_KEY_SIZE);
  MAM_ASSERT(trits_size(encrypted_session_key) == MAM_NTRU_EKEY_SIZE);

  bool ok = false;
  MAM_POLY_DEF(h);
  MAM_POLY_DEF(s);

  ok = poly_from_trits(h, public_key);
  MAM_ASSERT(ok);
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
}

bool ntru_decr(mam_ntru_sk_t const *const ntru, mam_spongos_t *const spongos,
               trits_t const encrypted_session_key, trits_t session_key) {
  MAM_ASSERT(trits_size(session_key) == MAM_NTRU_KEY_SIZE);
  MAM_ASSERT(trits_size(encrypted_session_key) == MAM_NTRU_EKEY_SIZE);

  bool b;
  poly_coeff_t *f;
  MAM_POLY_DEF(s);
  MAM_POLY_DEF(r);
  MAM_TRITS_DEF0(kt, MAM_NTRU_SK_SIZE);
  MAM_TRITS_DEF0(rh, MAM_NTRU_EKEY_SIZE);
  MAM_TRITS_DEF0(m, MAM_NTRU_SK_SIZE - MAM_NTRU_KEY_SIZE);
  kt = MAM_TRITS_INIT(kt, MAM_NTRU_SK_SIZE);
  rh = MAM_TRITS_INIT(rh, MAM_NTRU_EKEY_SIZE);
  m = MAM_TRITS_INIT(m, MAM_NTRU_SK_SIZE - MAM_NTRU_KEY_SIZE);

  /* f is NTT form */
  f = (poly_coeff_t *)ntru->f;

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
  b = trits_cmp_eq(m, trits_drop(kt, MAM_NTRU_KEY_SIZE));

  memset_safe(trits_begin(kt), trits_size(kt), 0, trits_size(kt));
  memset_safe(trits_begin(rh), trits_size(rh), 0, trits_size(rh));
  memset_safe(trits_begin(m), trits_size(m), 0, trits_size(m));
  memset_safe(s, sizeof(s), 0, sizeof(s));
  memset_safe(r, sizeof(r), 0, sizeof(r));

  return b;
}

void ntru_load_sk(mam_ntru_sk_t *n) {
  poly_coeff_t *f = (poly_coeff_t *)n->f;

  poly_small_from_trits(f, ntru_sk_trits(n));
  /* f := NTT(1+3f) */
  poly_small_mul3(f, f);
  poly_small3_add1(f);
  poly_ntt(f, f);
}
