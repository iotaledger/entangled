/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/sponge.h"
#include "common/trinary/add.h"
#include "mam/v2/buffers.h"

static trits_t sponge_state_trits(sponge_t *s) {
  return trits_from_rep(MAM2_SPONGE_WIDTH, s->s);
}

static trits_t sponge_outer1_trits(sponge_t *s) {
  // one extra trit for padding
  return trits_take(sponge_state_trits(s), MAM2_SPONGE_RATE + 1);
}

static inline void set_control_tryte(sponge_t *s, size_t i, trit_t c0,
                                     trit_t c1, trit_t c2) {
  trits_t t = trits_drop(sponge_state_trits(s), MAM2_SPONGE_RATE + 3 * i);
  t.p[t.d] = c0;
  t.p[t.d + 1] = c1;
  t.p[t.d + 2] = c2;
}

trits_t sponge_outer_trits(sponge_t *s, size_t n) {
  MAM2_ASSERT(n <= MAM2_SPONGE_RATE);
  return trits_take(sponge_state_trits(s), n);
}

#define MAM2_SPONGE_IS_CONTROL_TRIT_ZERO(S, i) \
  (trits_get1(trits_drop(sponge_state_trits(S), MAM2_SPONGE_RATE + i)) == 0)

static void sponge_transform(sponge_t *s) { s->f(s->stack, s->s); }

void sponge_fork(sponge_t *s, sponge_t *fork) {
  trits_copy(sponge_state_trits(s), sponge_state_trits(fork));
}

void sponge_init(sponge_t *s) { trits_set_zero(sponge_state_trits(s)); }

void sponge_absorb(sponge_t *s, trit_t c2, trits_t X) {
  size_t num_trits = X.n - X.d;
  TRIT_ARRAY_MAKE_FROM_RAW(X_arr, num_trits, X.p + X.d);
  sponge_absorb_flex(s, c2, &X_arr);
  flex_trits_to_trits(&X.p[X.d], num_trits, X_arr.trits, num_trits, num_trits);
}

void sponge_absorb_flex(sponge_t *s, trit_t c2, trit_array_p X_arr) {
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
}

void sponge_squeeze(sponge_t *s, trit_t c2, trits_t Y) {
  size_t num_trits = Y.n - Y.d;
  TRIT_ARRAY_DECLARE(Y_arr, num_trits);
  sponge_squeeze_flex(s, c2, &Y_arr);
  flex_trits_to_trits(&Y.p[Y.d], num_trits, Y_arr.trits, num_trits, num_trits);
}

void sponge_squeeze_flex(sponge_t *s, trit_t c2, trit_array_p Y_arr) {
  trits_t Yi;
  size_t ni;
  trit_t c0 = -1, c1;
  trit_t y_rep[Y_arr->num_trits];
  trits_t Y = trits_from_rep(Y_arr->num_trits, y_rep);
  trit_t *y_p = Y.p;
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

  flex_trits_from_trits(Y_arr->trits, Y_arr->num_trits, y_p, Y_arr->num_trits,
                        Y_arr->num_trits);
}

void sponge_absorbn_flex(sponge_t *s, trit_t c2, size_t n, trit_array_t Xs[]) {
  size_t total_trits = 0;
  for (size_t i = 0; i < n; ++i) {
    total_trits += Xs[i].num_trits;
  }
  trit_t buffer[total_trits + NUM_TRITS_PER_FLEX_TRIT];
  trits_t Xs_trits[n];
  size_t trits_pos = 0;
  for (size_t i = 0; i < n; ++i) {
    flex_trits_to_trits(&buffer[trits_pos], Xs[i].num_trits, Xs[i].trits,
                        Xs[i].num_trits, Xs[i].num_trits);
    Xs_trits[i] = trits_from_rep(Xs[i].num_trits, &buffer[trits_pos]);
    trits_pos += Xs[i].num_trits;
  }

  buffers_t tb = buffers_init(n, Xs_trits);
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

void sponge_encr(sponge_t *s, trits_t X, trits_t Y) {
  size_t y_size = Y.n - Y.d;
  TRIT_ARRAY_MAKE_FROM_RAW(X_arr, X.n, X.p + X.d);
  TRIT_ARRAY_DECLARE(Y_arr, y_size);
  sponge_encr_flex(s, &X_arr, &Y_arr);
  flex_trits_to_trits(Y.p + Y.d, y_size, Y_arr.trits, y_size, y_size);
}

void sponge_encr_flex(sponge_t *s, trit_array_p X_arr, trit_array_p Y_arr) {
  trits_t Xi, Yi;
  size_t ni;
  trit_t c0, c1, c2 = -1;
  MAM2_ASSERT(X_arr->num_trits == Y_arr->num_trits);
  trit_t x_rep[X_arr->num_trits + NUM_TRITS_PER_FLEX_TRIT];
  trit_t y_rep[Y_arr->num_trits + NUM_TRITS_PER_FLEX_TRIT];
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

void sponge_decr(sponge_t *s, trits_t Y, trits_t X) {
  size_t x_size = X.n - X.d;
  TRIT_ARRAY_DECLARE(X_arr, x_size);
  TRIT_ARRAY_MAKE_FROM_RAW(Y_arr, Y.n, Y.p + Y.d);
  sponge_decr_flex(s, &X_arr, &Y_arr);
  flex_trits_to_trits(X.p + X.d, x_size, X_arr.trits, x_size, x_size);
}

void sponge_decr_flex(sponge_t *s, trit_array_p X_arr, trit_array_p Y_arr) {
  trits_t Xi, Yi;
  size_t ni;
  trit_t c0, c1, c2 = -1;
  trit_t x_rep[X_arr->num_trits + NUM_TRITS_PER_FLEX_TRIT];
  trit_t y_rep[Y_arr->num_trits + NUM_TRITS_PER_FLEX_TRIT];
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

void sponge_hash(sponge_t *s, trit_array_p X, trit_array_p Y) {
  sponge_init(s);
  sponge_absorb_flex(s, MAM2_SPONGE_CTL_HASH, X);
  sponge_squeeze_flex(s, MAM2_SPONGE_CTL_HASH, Y);
}

void sponge_hashn(sponge_t *s, size_t n, trit_array_t *Xs,
                  trit_array_p Y) {
  sponge_init(s);
  sponge_absorbn_flex(s, MAM2_SPONGE_CTL_HASH, n, Xs);
  sponge_squeeze_flex(s, MAM2_SPONGE_CTL_HASH, Y);
}
