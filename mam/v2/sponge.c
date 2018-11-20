
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
\file sponge.c
\brief MAM2 Sponge layer.
*/
#include "mam/v2/sponge.h"
#include <stdlib.h>
#include "common/trinary/add.h"
#include "common/trinary/trit_array.h"
#include "mam/v2/buffers.h"

static trits_t sponge_state_trits(isponge *s) {
  return trits_from_rep(MAM2_SPONGE_WIDTH, s->s);
}

static trits_t sponge_outer1_trits(isponge *s) {
  // one extra trit for padding
  return trits_take(sponge_state_trits(s), MAM2_SPONGE_RATE + 1);
}

static inline void set_control_tryte(isponge *s, size_t i, trit_t c0, trit_t c1,
                                     trit_t c2) {
  trits_t t = trits_drop(sponge_state_trits(s), MAM2_SPONGE_RATE + 3 * i);
  t.p[t.d] = c0;
  t.p[t.d + 1] = c1;
  t.p[t.d + 2] = c2;
}

static trits_t sponge_control_trits(isponge *s) {
  return trits_take(trits_drop(sponge_state_trits(s), MAM2_SPONGE_RATE),
                    MAM2_SPONGE_CONTROL);
}

trits_t sponge_outer_trits(isponge *s, size_t n) {
  MAM2_ASSERT(n <= MAM2_SPONGE_RATE);
  return trits_take(sponge_state_trits(s), n);
}

#define MAM2_SPONGE_IS_CONTROL_TRIT_ZERO(S, i) \
  (trits_get1(trits_drop(sponge_state_trits(S), MAM2_SPONGE_RATE + i)) == 0)

static void sponge_transform(isponge *s) { s->f(s->stack, s->s); }

void sponge_fork(isponge *s, isponge *fork) {
  trits_copy(sponge_state_trits(s), sponge_state_trits(fork));
}

void sponge_init(isponge *s) { trits_set_zero(sponge_state_trits(s)); }

void sponge_absorb(isponge *s, trit_t c2, trits_t X) {
  size_t num_trits = X.n - X.d;
  TRIT_ARRAY_MAKE_FROM_RAW(X_arr, num_trits, X.p);
  sponge_absorb_flex(s, c2, &X_arr);
  flex_trits_to_trits(&X.p[X.d], num_trits, X_arr.trits, num_trits, num_trits);
}

void sponge_absorb_flex(isponge *s, trit_t c2, trit_array_p X_arr) {
  trits_t Xi;
  size_t ni;
  trit_t c0, c1;

  trit_t x_rep[X_arr->num_trits];
  flex_trits_to_trits(x_rep, X_arr->num_trits, X_arr->trits, X_arr->num_trits,
                      X_arr->num_trits);

  trits_t X = trits_from_rep(X_arr->num_trits, x_rep);
  trit_t *x_p = X.p;

  do {
    Xi = trits_take_min(X, MAM2_SPONGE_RATE);
    c0 = (trits_size(X) < MAM2_SPONGE_RATE) ? 0 : 1;
    ni = trits_size(Xi);
    X = trits_drop(X, ni);
    c1 = trits_is_empty(X) ? 1 : -1;
    if (!MAM2_SPONGE_IS_CONTROL_TRIT_ZERO(s, 1)) {
      set_control_tryte(s, 1, c0, c1, c2);
      sponge_transform(s);
    }
    trits_copy_pad10(Xi, sponge_outer1_trits(s));
    set_control_tryte(s, 0, c0, c1, c2);
  } while (!trits_is_empty(X));

  flex_trits_from_trits(X_arr->trits, X_arr->num_trits, x_p, X_arr->num_trits,
                        X_arr->num_trits);
}

void sponge_squeeze(isponge *s, trit_t c2, trits_t Y) {
  trits_t Yi;
  size_t ni;
  trit_t c0 = -1, c1;
  do {
    Yi = trits_take_min(Y, MAM2_SPONGE_RATE);
    c1 = (trits_size(Y) < MAM2_SPONGE_RATE) ? 0 : 1;
    ni = trits_size(Yi);
    Y = trits_drop(Y, ni);
    set_control_tryte(s, 1, c0, c1, c2);
    sponge_transform(s);
    trits_copy(sponge_outer_trits(s, ni), Yi);
    set_control_tryte(s, 0, c0, c1, c2);
  } while (!trits_is_empty(Y));
}

void sponge_absorbn(isponge *s, trit_t c2, size_t n, trits_t *Xs) {
  buffers_t tb = buffers_init(n, Xs);
  size_t m = buffers_size(tb);
  trits_t s1 = sponge_outer1_trits(s);
  size_t ni;
  trit_t c0, c1;
  do {
    ni = (m < MAM2_SPONGE_RATE) ? m : MAM2_SPONGE_RATE;
    m -= ni;
    c0 = (ni < MAM2_SPONGE_RATE) ? 0 : 1;
    c1 = (0 == m) ? 1 : -1;
    if (!MAM2_SPONGE_IS_CONTROL_TRIT_ZERO(s, 1)) {
      set_control_tryte(s, 1, c0, c1, c2);
      sponge_transform(s);
    }
    buffers_copy_to(&tb, trits_take(s1, ni));
    trits_pad10(trits_drop(s1, ni));
    set_control_tryte(s, 0, c0, c1, c2);
  } while (0 < m);
}

void sponge_encr(isponge *s, trits_t X, trits_t Y) {
  TRIT_ARRAY_MAKE_FROM_RAW(X_arr, X.n, X.p);
  TRIT_ARRAY_MAKE_FROM_RAW(Y_arr, Y.n, Y.p);
  sponge_encr_flex(s, &X_arr, &Y_arr);
  flex_trits_to_trits(Y.p, Y_arr.num_trits, Y_arr.trits, Y_arr.num_trits,
                      Y_arr.num_trits);
}

void sponge_encr_flex(isponge *s, trit_array_p X_arr, trit_array_p Y_arr) {
  trits_t Xi, Yi;
  size_t ni;
  trit_t c0, c1, c2 = -1;
  MAM2_ASSERT(X_arr->num_trits == Y_arr->num_trits);
  trit_t x_rep[X_arr->num_trits];
  trit_t y_rep[Y_arr->num_trits];
  flex_trits_to_trits(x_rep, X_arr->num_trits, X_arr->trits, X_arr->num_trits,
                      X_arr->num_trits);
  flex_trits_to_trits(y_rep, Y_arr->num_trits, Y_arr->trits, Y_arr->num_trits,
                      Y_arr->num_trits);

  trits_t X = trits_from_rep(X_arr->num_trits, x_rep);
  trits_t Y = trits_from_rep(Y_arr->num_trits, y_rep);
  trit_t *y_p = Y.p;

  do {
    Xi = trits_take_min(X, MAM2_SPONGE_RATE);
    Yi = trits_take_min(Y, MAM2_SPONGE_RATE);
    c0 = (trits_size(X) < MAM2_SPONGE_RATE) ? 0 : 1;
    ni = trits_size(Xi);
    X = trits_drop(X, ni);
    Y = trits_drop(Y, ni);
    c1 = trits_is_empty(X) ? 1 : -1;
    set_control_tryte(s, 1, c0, c1, c2);
    sponge_transform(s);
    if (trits_is_same(Xi, Yi)) {
      trits_swap_add(Xi, sponge_outer_trits(s, ni));
    } else {
      trits_copy_add(Xi, sponge_outer_trits(s, ni), Yi);
    }
    trits_pad10(trits_drop(sponge_outer1_trits(s), ni));
    set_control_tryte(s, 0, c0, c1, c2);
  } while (!trits_is_empty(X));
  flex_trits_from_trits(Y_arr->trits, Y_arr->num_trits, y_p, Y_arr->num_trits,
                        Y_arr->num_trits);
}

void sponge_decr(isponge *s, trits_t Y, trits_t X) {
  TRIT_ARRAY_MAKE_FROM_RAW(X_arr, X.n, X.p);
  TRIT_ARRAY_MAKE_FROM_RAW(Y_arr, Y.n, Y.p);
  sponge_decr_flex(s, &X_arr, &Y_arr);
  flex_trits_to_trits(X.p, X_arr.num_trits, X_arr.trits, X_arr.num_trits,
                      X_arr.num_trits);
}

void sponge_decr_flex(isponge *s, trit_array_p X_arr, trit_array_p Y_arr) {
  trits_t Xi, Yi;
  size_t ni;
  trit_t c0, c1, c2 = -1;
  trit_t x_rep[X_arr->num_trits];
  trit_t y_rep[Y_arr->num_trits];
  flex_trits_to_trits(x_rep, X_arr->num_trits, X_arr->trits, X_arr->num_trits,
                      X_arr->num_trits);
  flex_trits_to_trits(y_rep, Y_arr->num_trits, Y_arr->trits, Y_arr->num_trits,
                      Y_arr->num_trits);

  trits_t X = trits_from_rep(X_arr->num_trits, x_rep);
  trits_t Y = trits_from_rep(Y_arr->num_trits, y_rep);
  trit_t *x_p = X.p;

  do {
    Xi = trits_take_min(X, MAM2_SPONGE_RATE);
    Yi = trits_take_min(Y, MAM2_SPONGE_RATE);
    c0 = (trits_size(X) < MAM2_SPONGE_RATE) ? 0 : 1;
    ni = trits_size(Xi);
    X = trits_drop(X, ni);
    Y = trits_drop(Y, ni);
    c1 = trits_is_empty(X) ? 1 : -1;
    set_control_tryte(s, 1, c0, c1, c2);
    sponge_transform(s);
    if (trits_is_same(Xi, Yi)) {
      trits_swap_sub(Xi, sponge_outer_trits(s, ni));
    } else {
      trits_copy_sub(Yi, sponge_outer_trits(s, ni), Xi);
    }

    trits_pad10(trits_drop(sponge_outer1_trits(s), ni));
    set_control_tryte(s, 0, c0, c1, c2);
  } while (!trits_is_empty(Y));
  flex_trits_from_trits(X_arr->trits, X_arr->num_trits, x_p, X_arr->num_trits,
                        X_arr->num_trits);
}

void sponge_hash(isponge *s, trits_t X, trits_t Y) {
  sponge_init(s);
  sponge_absorb(s, MAM2_SPONGE_CTL_HASH, X);
  sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, Y);
}

void sponge_hashn(isponge *s, size_t n, trits_t *Xs, trits_t Y) {
  sponge_init(s);
  sponge_absorbn(s, MAM2_SPONGE_CTL_HASH, n, Xs);
  sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, Y);
}
