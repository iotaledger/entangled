/*!
\file wots.c
\brief MAM2 WOTS layer.
*/
#include "wots.h"

#include <string.h>

static void wots_calc_pks(ispongos *s, trits_t sk_pks, trits_t pk) {
  size_t i, j;
  trits_t sk_part, pks = sk_pks;

  MAM2_ASSERT(trits_size(sk_pks) == MAM2_WOTS_SK_SIZE);
  MAM2_ASSERT(trits_size(pk) == MAM2_WOTS_PK_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    sk_part = trits_take(sk_pks, MAM2_WOTS_SK_PART_SIZE);
    sk_pks = trits_drop(sk_pks, MAM2_WOTS_SK_PART_SIZE);

    for (j = 0; j < 26; ++j) spongos_hash(s, sk_part, sk_part);
  }

  spongos_hash(s, pks, pk);
}

static void wots_hash_sign(ispongos *s, trits_t sk_sig, trits_t H) {
  size_t i;
  trint9_t t = 0;
  trint3_t j, h;
  trits_t sig_part;

  MAM2_ASSERT(trits_size(sk_sig) == MAM2_WOTS_SK_SIZE);
  MAM2_ASSERT(trits_size(H) == MAM2_WOTS_HASH_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT - 3; ++i) {
    sig_part = trits_take(sk_sig, MAM2_WOTS_SK_PART_SIZE);
    sk_sig = trits_drop(sk_sig, MAM2_WOTS_SK_PART_SIZE);

    h = trits_get3(trits_drop(H, i * 3));
    t += h;

    for (j = -13; j < h; ++j) spongos_hash(s, sig_part, sig_part);
  }

  t = -t;
  for (; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    sig_part = trits_take(sk_sig, MAM2_WOTS_SK_PART_SIZE);
    sk_sig = trits_drop(sk_sig, MAM2_WOTS_SK_PART_SIZE);

    h = MAM2_MODS(t, 19683, 27);
    t = MAM2_DIVS(t, 19683, 27);

    for (j = -13; j < h; ++j) spongos_hash(s, sig_part, sig_part);
  }
}

static void wots_hash_verify(ispongos *s, trits_t sig_pks, trits_t H) {
  size_t i;
  trint9_t t = 0;
  trint3_t j, h;
  trits_t sig_part;

  MAM2_ASSERT(trits_size(sig_pks) == MAM2_WOTS_SK_SIZE);
  MAM2_ASSERT(trits_size(H) == MAM2_WOTS_HASH_SIZE);

  for (i = 0; i < MAM2_WOTS_SK_PART_COUNT - 3; ++i) {
    sig_part = trits_take(sig_pks, MAM2_WOTS_SK_PART_SIZE);
    sig_pks = trits_drop(sig_pks, MAM2_WOTS_SK_PART_SIZE);

    h = trits_get3(trits_drop(H, i * 3));
    t += h;

    for (j = -13; j < -h; ++j) spongos_hash(s, sig_part, sig_part);
  }

  t = -t;
  for (; i < MAM2_WOTS_SK_PART_COUNT; ++i) {
    sig_part = trits_take(sig_pks, MAM2_WOTS_SK_PART_SIZE);
    sig_pks = trits_drop(sig_pks, MAM2_WOTS_SK_PART_SIZE);

    h = MAM2_MODS(t, 19683, 27);
    t = MAM2_DIVS(t, 19683, 27);

    for (j = -13; j < -h; ++j) spongos_hash(s, sig_part, sig_part);
  }
}

static trits_t wots_sk_trits(iwots *w) {
  return trits_from_rep(MAM2_WOTS_SK_SIZE, w->sk);
}

void wots_init(iwots *w, isponge *s) {
  MAM2_ASSERT(w);
  MAM2_ASSERT(s);
  w->sg->s = s;
}

void wots_gen_sk(iwots *w, iprng *p, trits_t N) {
  wots_gen_sk3(w, p, N, trits_null(), trits_null());
}

void wots_gen_sk2(iwots *w, iprng *p, trits_t N1, trits_t N2) {
  wots_gen_sk3(w, p, N1, N2, trits_null());
}

void wots_gen_sk3(iwots *w, iprng *p, trits_t N1, trits_t N2, trits_t N3) {
  prng_gen3(p, MAM2_PRNG_DST_WOTSKEY, N1, N2, N3, wots_sk_trits(w));
}

void wots_calc_pk(iwots *w, trits_t pk) {
  MAM2_TRITS_DEF0(sk_pks, MAM2_WOTS_SK_SIZE);
  sk_pks = MAM2_TRITS_INIT(sk_pks, MAM2_WOTS_SK_SIZE);

  trits_copy(wots_sk_trits(w), sk_pks);
  wots_calc_pks(w->sg, sk_pks, pk);
  trits_set_zero(sk_pks);
}

void wots_sign(iwots *w, trits_t H, trits_t sig) {
  trits_copy(wots_sk_trits(w), sig);
  wots_hash_sign(w->sg, sig, H);
}

void wots_recover(ispongos *s, trits_t H, trits_t sig, trits_t pk) {
  MAM2_TRITS_DEF0(sig_pks, MAM2_WOTS_SK_SIZE);
  sig_pks = MAM2_TRITS_INIT(sig_pks, MAM2_WOTS_SK_SIZE);

  MAM2_ASSERT(trits_size(pk) == MAM2_WOTS_PK_SIZE);

  trits_copy(sig, sig_pks);
  wots_hash_verify(s, sig_pks, H);
  spongos_hash(s, sig_pks, pk);
}

bool_t wots_verify(ispongos *s, trits_t H, trits_t sig, trits_t pk) {
  MAM2_TRITS_DEF0(sig_pk, MAM2_WOTS_PK_SIZE);
  sig_pk = MAM2_TRITS_INIT(sig_pk, MAM2_WOTS_PK_SIZE);

  wots_recover(s, H, sig, sig_pk);
  return trits_cmp_eq(pk, sig_pk);
}

err_t wots_create(ialloc *a, iwots *w) {
  err_t e = err_internal_error;
  MAM2_ASSERT(w);
  do {
    memset(w, 0, sizeof(iwots));
    w->sk = mam_words_alloc(a, MAM2_WORDS(MAM2_WOTS_SK_SIZE));
    err_guard(w->sk, err_bad_alloc);
    e = err_ok;
  } while (0);
  return e;
}

void wots_destroy(ialloc *a, iwots *w) {
  MAM2_ASSERT(w);
  mam_words_free(a, w->sk);
  w->sk = 0;
}

bool_t wots_test(iwots *w, iprng *p) {
  bool_t r = 1;
  MAM2_TRITS_DEF0(N, 18);
  MAM2_TRITS_DEF0(pk, MAM2_WOTS_PK_SIZE);
  MAM2_TRITS_DEF0(H, MAM2_WOTS_HASH_SIZE);
  MAM2_TRITS_DEF0(sig, MAM2_WOTS_SIG_SIZE);
  /*MAM2_TRITS_DEF0(pkr, MAM2_WOTS_PK_SIZE);*/
  N = MAM2_TRITS_INIT(N, 18);
  pk = MAM2_TRITS_INIT(pk, MAM2_WOTS_PK_SIZE);
  H = MAM2_TRITS_INIT(H, MAM2_WOTS_HASH_SIZE);
  sig = MAM2_TRITS_INIT(sig, MAM2_WOTS_SIG_SIZE);
  /*pkr = MAM2_TRITS_INIT(pkr, MAM2_WOTS_PK_SIZE);*/

  trits_set_zero(N);
  trits_set_zero(H);
  trits_set_zero(wots_sk_trits(w));

  prng_gen(p, 7, N, H);
  wots_gen_sk(w, p, N);
  wots_calc_pk(w, pk);
  wots_sign(w, H, sig);
  /*wots_recover(w->sg, H, sig, pkr);*/
  r = r && wots_verify(w->sg, H, sig, pk);

  trits_put1(H, trit_add(trits_get1(H), 1));
  r = r && !wots_verify(w->sg, H, sig, pk);
  trits_put1(H, trit_sub(trits_get1(H), 1));

  trits_put1(sig, trit_add(trits_get1(sig), 1));
  r = r && !wots_verify(w->sg, H, sig, pk);
  trits_put1(sig, trit_sub(trits_get1(sig), 1));

  trits_put1(pk, trit_add(trits_get1(pk), 1));
  r = r && !wots_verify(w->sg, H, sig, pk);
  trits_put1(pk, trit_sub(trits_get1(pk), 1));

  return r;
}
