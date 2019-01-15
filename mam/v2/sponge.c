/*!
\file sponge.c
\brief MAM2 Sponge layer.
*/
#include "sponge.h"

#include "buffers.h"

static trits_t sponge_state_trits(isponge *s) {
  return trits_from_rep(MAM2_SPONGE_WIDTH, s->s);
}

static trits_t sponge_control_trits(isponge *s) {
  return trits_take(trits_drop(sponge_state_trits(s), MAM2_SPONGE_RATE),
                    MAM2_SPONGE_CONTROL);
}

trits_t sponge_outer_trits(isponge *s) {
  return trits_take(sponge_state_trits(s), MAM2_SPONGE_RATE);
}

static void sponge_set_control12(isponge *s, trit_t c1, trit_t c2) {
  trits_t t = trits_take(sponge_control_trits(s), 3);
  trits_drop(t, 1);
  trits_put1(t, c1);
  trits_drop(t, 1);
  trits_put1(t, c2);
}

static void sponge_set_control012(isponge *s, trit_t c0, trit_t c1, trit_t c2) {
  trits_t t = trits_take(sponge_control_trits(s), 3);
  trits_put1(t, c0);
  trits_drop(t, 1);
  trits_put1(t, c1);
  trits_drop(t, 1);
  trits_put1(t, c2);
}

static void sponge_set_control345(isponge *s, trit_t c3, trit_t c4, trit_t c5) {
  trits_t t = trits_drop(sponge_control_trits(s), 3);
  trits_put1(t, c3);
  trits_drop(t, 1);
  trits_put1(t, c4);
  trits_drop(t, 1);
  trits_put1(t, c5);
}

static trit_t sponge_get_control1(isponge *s) {
  return trits_get1(trits_drop(sponge_control_trits(s), 1));
}

void sponge_transform(isponge *s) { s->f(s->stack, s->s); }

void sponge_fork(isponge *s, isponge *fork) {
  trits_copy(sponge_state_trits(s), sponge_state_trits(fork));
}

void sponge_init(isponge *s) { trits_set_zero(sponge_state_trits(s)); }

void sponge_absorb(isponge *s, trit_t c2, trits_t X) {
  trits_t s1;
  trits_t Xi;
  size_t ni;
  trit_t c0 = 1, c1;

  MAM2_ASSERT(c2 == MAM2_SPONGE_CTL_DATA || c2 == MAM2_SPONGE_CTL_KEY);

  s1 = sponge_outer_trits(s);

  do {
    Xi = trits_take_min(X, MAM2_SPONGE_RATE);
    ni = trits_size(Xi);
    X = trits_drop(X, ni);
    c0 = (ni < MAM2_SPONGE_RATE) ? 0 : 1;
    c1 = trits_is_empty(X) ? -1 : 1;

    if (0 != sponge_get_control1(s)) {
      sponge_set_control345(s, c0, c1, c2);
      sponge_transform(s);
    }

    trits_copy(Xi, trits_take(s1, ni));
    trits_padc(1, trits_drop(s1, ni));
    sponge_set_control012(s, c0, c1, c2);
  } while (!trits_is_empty(X));
}

void sponge_absorbn(isponge *s, trit_t c2, size_t n, trits_t *Xs) {
  buffers_t tb;
  size_t m;
  trits_t s1;
  size_t ni;
  trit_t c0 = 1, c1;

  MAM2_ASSERT(c2 == MAM2_SPONGE_CTL_DATA || c2 == MAM2_SPONGE_CTL_KEY);

  tb = buffers_init(n, Xs);
  m = buffers_size(tb);
  s1 = sponge_outer_trits(s);

  do {
    ni = (m < MAM2_SPONGE_RATE) ? m : MAM2_SPONGE_RATE;
    m -= ni;
    c0 = (ni < MAM2_SPONGE_RATE) ? 0 : 1;
    c1 = (0 == m) ? -1 : 1;

    if (0 != sponge_get_control1(s)) {
      sponge_set_control345(s, c0, c1, c2);
      sponge_transform(s);
    }

    buffers_copy_to(&tb, trits_take(s1, ni));
    trits_padc(1, trits_drop(s1, ni));
    sponge_set_control012(s, c0, c1, c2);
  } while (0 < m);
}

void sponge_squeeze(isponge *s, trit_t c2, trits_t Y) {
  trits_t s1;
  trits_t Yi;
  size_t ni;
  trit_t c0 = -1, c1;

  s1 = sponge_outer_trits(s);

  do {
    Yi = trits_take_min(Y, MAM2_SPONGE_RATE);
    ni = trits_size(Yi);
    Y = trits_drop(Y, ni);
    c1 = trits_is_empty(Y) ? -1 : 1;

    sponge_set_control345(s, c0, c1, c2);
    sponge_transform(s);

    trits_copy(trits_take(s1, ni), Yi);
    trits_set_zero(trits_take(s1, ni));
    trits_padc(1, trits_drop(s1, ni));
    sponge_set_control012(s, c0, c1, c2);
  } while (!trits_is_empty(Y));
}

void sponge_encr(isponge *s, trits_t X, trits_t Y) {
  trits_t s1;
  trits_t Xi, Yi;
  size_t ni;
  trit_t c0 = 1, c1, c2 = -1;

  MAM2_ASSERT(trits_size(X) == trits_size(Y));
  MAM2_ASSERT(trits_is_same(X, Y) || !trits_is_overlapped(X, Y));

  s1 = sponge_outer_trits(s);

  do {
    Xi = trits_take_min(X, MAM2_SPONGE_RATE);
    Yi = trits_take_min(Y, MAM2_SPONGE_RATE);
    ni = trits_size(Xi);
    X = trits_drop(X, ni);
    Y = trits_drop(Y, ni);
    c0 = (ni < MAM2_SPONGE_RATE) ? 0 : 1;
    c1 = trits_is_empty(X) ? -1 : 1;

    sponge_set_control345(s, c0, c1, c2);
    sponge_transform(s);

    if (trits_is_same(Xi, Yi))
      trits_swap_add(Xi, trits_take(s1, ni));
    else
      trits_copy_add(Xi, trits_take(s1, ni), Yi);
    trits_padc(1, trits_drop(s1, ni));
    sponge_set_control012(s, c0, c1, c2);
  } while (!trits_is_empty(X));
}

void sponge_decr(isponge *s, trits_t Y, trits_t X) {
  trits_t s1;
  trits_t Xi, Yi;
  size_t ni;
  trit_t c0 = 1, c1, c2 = -1;

  MAM2_ASSERT(trits_size(X) == trits_size(Y));
  MAM2_ASSERT(trits_is_same(X, Y) || !trits_is_overlapped(X, Y));

  s1 = sponge_outer_trits(s);

  do {
    Xi = trits_take_min(X, MAM2_SPONGE_RATE);
    Yi = trits_take_min(Y, MAM2_SPONGE_RATE);
    ni = trits_size(Xi);
    X = trits_drop(X, ni);
    Y = trits_drop(Y, ni);
    c0 = (ni < MAM2_SPONGE_RATE) ? 0 : 1;
    c1 = trits_is_empty(X) ? -1 : 1;

    sponge_set_control345(s, c0, c1, c2);
    sponge_transform(s);

    if (trits_is_same(Xi, Yi))
      trits_swap_sub(Yi, trits_take(s1, ni));
    else
      trits_copy_sub(Yi, trits_take(s1, ni), Xi);
    trits_padc(1, trits_drop(s1, ni));
    sponge_set_control012(s, c0, c1, c2);
  } while (!trits_is_empty(Y));
}

void sponge_hash(isponge *s, trits_t X, trits_t Y) {
  sponge_init(s);
  sponge_absorb(s, MAM2_SPONGE_CTL_DATA, X);
  sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, Y);
}

void sponge_hashn(isponge *s, size_t n, trits_t *Xs, trits_t Y) {
  sponge_init(s);
  sponge_absorbn(s, MAM2_SPONGE_CTL_DATA, n, Xs);
  sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, Y);
}

static bool_t sponge_test_hash(isponge *s) {
  bool_t r = 1;
  MAM2_TRITS_DEF0(X0, MAM2_SPONGE_RATE * 3);
  trits_t Xs[3], X;
  MAM2_TRITS_DEF0(Y1, 243);
  MAM2_TRITS_DEF0(Y2, 243);
  size_t n;
  X0 = MAM2_TRITS_INIT(X0, MAM2_SPONGE_RATE * 3);
  Y1 = MAM2_TRITS_INIT(Y1, 243);
  Y2 = MAM2_TRITS_INIT(Y2, 243);

  for (n = 0; n < MAM2_SPONGE_RATE * 3; n += MAM2_SPONGE_RATE / 2) {
    X = trits_take(X0, n);
    trits_set_zero(X);
    sponge_hash(s, X, Y1);

    if (0 && 0 < n) {
      trits_put1(X, trit_add(trits_get1(X), 1));
      sponge_hash(s, X, Y2);
      trits_put1(X, trit_sub(trits_get1(X), 1));
      r = r && !trits_cmp_eq(Y1, Y2);
    }

    Xs[0] = trits_take(X, n / 3);
    Xs[1] = trits_take(trits_drop(X, n / 3), n / 4);
    Xs[2] = trits_drop(X, n / 3 + n / 4);
    sponge_hashn(s, 3, Xs, Y2);
    r = r && trits_cmp_eq(Y1, Y2);
  }

  return r;
}

static bool_t sponge_test_ae(isponge *s) {
#define MAM2_SPONGE_TEST_MAX_K 1110
  size_t k, i;
  MAM2_TRITS_DEF0(K, MAM2_SPONGE_KEY_SIZE);
  MAM2_TRITS_DEF0(X, MAM2_SPONGE_TEST_MAX_K);
  MAM2_TRITS_DEF0(Y, MAM2_SPONGE_TEST_MAX_K);
  MAM2_TRITS_DEF0(Z, MAM2_SPONGE_TEST_MAX_K);
  size_t ks[] = {0,
                 1,
                 2,
                 3,
                 4,
                 5,
                 6,
                 242,
                 243,
                 244,
                 485,
                 486,
                 487,
                 MAM2_SPONGE_TEST_MAX_K,
                 MAM2_SPONGE_TEST_MAX_K + 1};
  K = MAM2_TRITS_INIT(K, MAM2_SPONGE_KEY_SIZE);
  X = MAM2_TRITS_INIT(X, MAM2_SPONGE_TEST_MAX_K);
  Y = MAM2_TRITS_INIT(Y, MAM2_SPONGE_TEST_MAX_K);
  Z = MAM2_TRITS_INIT(Z, MAM2_SPONGE_TEST_MAX_K);

  /* init X */
  for (i = 0; i < MAM2_SPONGE_TEST_MAX_K / 3; ++i)
    trits_put3(trits_drop(X, 3 * i), (tryte_t)((i % 27) - 13));
  /* init K */
  trits_set_zero(K);
  trits_from_str(K,
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM");
  sponge_init(s);
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
  sponge_squeeze(s, MAM2_SPONGE_CTL_PRN, K);

  for (i = 0; (k = ks[i++]) <= MAM2_SPONGE_TEST_MAX_K;) {
    X = MAM2_TRITS_INIT(X, k);
    Y = MAM2_TRITS_INIT(Y, k);
    Z = MAM2_TRITS_INIT(Z, k);

    sponge_init(s);
    sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
    sponge_encr(s, X, Y); /* Y=E(X) */

    sponge_init(s);
    sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
    sponge_decr(s, Y, Z); /* Z=D(E(X)) */
    if (!trits_cmp_eq(X, Z)) return 0;

    sponge_init(s);
    sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
    sponge_encr(s, Z, Z); /* Z=E(Z=X) */
    if (!trits_cmp_eq(Y, Z)) return 0;

    sponge_init(s);
    sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
    sponge_decr(s, Z, Z); /* Z=D(Z=E(X)) */
    if (!trits_cmp_eq(X, Z)) return 0;
  }

#undef MAM2_SPONGE_TEST_MAX_K
  return 1;
}

bool_t sponge_test(isponge *s) {
  return 1 && sponge_test_ae(s) && sponge_test_hash(s);
}
