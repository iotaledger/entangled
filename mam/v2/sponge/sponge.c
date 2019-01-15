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
\file sponge.c
\brief MAM2 Sponge layer.
*/
#include "mam/v2/sponge/sponge.h"
#include "mam/v2/trits/buffers.h"

static trits_t sponge_state_trits(sponge_t *s) {
  return trits_from_rep(MAM2_SPONGE_WIDTH, s->s);
}

static trits_t sponge_control_trits(sponge_t *s) {
  return trits_take(trits_drop(sponge_state_trits(s), MAM2_SPONGE_RATE),
                    MAM2_SPONGE_CONTROL);
}

trits_t sponge_outer_trits(sponge_t *s) {
  return trits_take(sponge_state_trits(s), MAM2_SPONGE_RATE);
}

static void sponge_set_control12(sponge_t *s, trit_t c1, trit_t c2) {
  trits_t t = trits_take(sponge_control_trits(s), 3);
  trits_drop(t, 1);
  trits_put1(t, c1);
  trits_drop(t, 1);
  trits_put1(t, c2);
}

static void sponge_set_control012(sponge_t *s, trit_t c0, trit_t c1,
                                  trit_t c2) {
  trits_t t = trits_take(sponge_control_trits(s), 3);
  trits_put1(t, c0);
  trits_drop(t, 1);
  trits_put1(t, c1);
  trits_drop(t, 1);
  trits_put1(t, c2);
}

static void sponge_set_control345(sponge_t *s, trit_t c3, trit_t c4,
                                  trit_t c5) {
  trits_t t = trits_drop(sponge_control_trits(s), 3);
  trits_put1(t, c3);
  trits_drop(t, 1);
  trits_put1(t, c4);
  trits_drop(t, 1);
  trits_put1(t, c5);
}

static trit_t sponge_get_control1(sponge_t *s) {
  return trits_get1(trits_drop(sponge_control_trits(s), 1));
}

void sponge_transform(sponge_t *s) { s->f(s->stack, s->s); }

void sponge_fork(sponge_t *s, sponge_t *fork) {
  trits_copy(sponge_state_trits(s), sponge_state_trits(fork));
}

void sponge_init(sponge_t *s) { trits_set_zero(sponge_state_trits(s)); }

void sponge_absorb(sponge_t *s, trit_t c2, trits_t X) {
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

void sponge_absorbn(sponge_t *s, trit_t c2, size_t n, trits_t *Xs) {
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

void sponge_squeeze(sponge_t *s, trit_t c2, trits_t Y) {
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

void sponge_encr(sponge_t *s, trits_t X, trits_t Y) {
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

void sponge_decr(sponge_t *s, trits_t Y, trits_t X) {
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

void sponge_hash(sponge_t *s, trits_t X, trits_t Y) {
  sponge_init(s);
  sponge_absorb(s, MAM2_SPONGE_CTL_DATA, X);
  sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, Y);
}

void sponge_hashn(sponge_t *s, size_t n, trits_t *Xs, trits_t Y) {
  sponge_init(s);
  sponge_absorbn(s, MAM2_SPONGE_CTL_DATA, n, Xs);
  sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, Y);
}
