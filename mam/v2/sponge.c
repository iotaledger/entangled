
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

/// XOR's two arrays - all arrays should be of the same size!
/// @param[in] y - the left operand
/// @param[in] copy_to - the right operand
/// @param[in] add_xor_to - the result array
/// @return trit_array_p - the receiver
trit_array_p trit_array_xor_sub(trit_array_p const y, trit_array_p const state,
                                trit_array_p const x);

/// XOR's two arrays (second operand negated) - all arrays should be of the same
/// size!
/// @param[in] plaintext - the left operand
/// @param[in] copy_res - the right operand
/// @param[in] sub_xor_res - the result array
/// @return trit_array_p - the receiver
trit_array_p trit_array_copy_add_xor(trit_array_p const plaintext,
                                     trit_array_p const state,
                                     trit_array_p const ciphertext) {
  assert(plaintext->num_trits == state->num_trits);
  assert(plaintext->num_trits == ciphertext->num_trits);

  trit_t lhs_trit;
  trit_t rhs_trit;

  for (size_t idx = 0; idx < ciphertext->num_trits; ++idx) {
    lhs_trit = trit_array_at(plaintext, idx);
    rhs_trit = trit_array_at(state, idx);
    trit_array_set_at(ciphertext, idx, trit_sum(lhs_trit, rhs_trit));
    trit_array_set_at(state, idx, lhs_trit);
  }
  return ciphertext;
}

trit_array_p trit_array_xor_sub(trit_array_p const y, trit_array_p const state,
                                trit_array_p const x) {
  assert(y->num_trits == state->num_trits);
  assert(y->num_trits == x->num_trits);

  trit_t lhs_trit;
  trit_t rhs_trit;

  for (size_t idx = 0; idx < x->num_trits; ++idx) {
    lhs_trit = trit_array_at(y, idx);
    rhs_trit = -trit_array_at(state, idx);
    trit_t res = trit_sum(lhs_trit, rhs_trit);
    trit_array_set_at(x, idx, res);
    trit_array_set_at(state, idx, res);
  }
  return x;
}

/*! \brief Update `i`-th control tryte of the state `S` with (`c0`,`c1`,`c2`).
 */
inline static void set_control_trits(isponge *s, size_t i, trit_t c0, trit_t c1,
                                     trit_t c2) {
  flex_trits_set_at(s->state, MAM2_SPONGE_WIDTH, MAM2_SPONGE_RATE + 3 * i, c0);
  flex_trits_set_at(s->state, MAM2_SPONGE_WIDTH, MAM2_SPONGE_RATE + 3 * i + 1,
                    c1);
  flex_trits_set_at(s->state, MAM2_SPONGE_WIDTH, MAM2_SPONGE_RATE + 3 * i + 2,
                    c2);
}

inline static void pad_1_and_0s(isponge *s, size_t i) {
  trit_array_t state;
  state.dynamic = 0;
  trit_array_set_trits(&state, s->state, MAM2_SPONGE_WIDTH);
  trit_array_set_at(&state, i, 1);
  trit_array_set_range(&state, i + 1,
                       MAM2_SPONGE_WIDTH - (MAM2_SPONGE_RATE + 1), 0);
}

bool static inline is_control_trit_zero(isponge *s, size_t i) {
  return flex_trits_at(s->state, MAM2_SPONGE_WIDTH, i);
}

static void sponge_transform(isponge *s) {
  trit_t state_trits[MAM2_SPONGE_WIDTH];
  flex_trits_to_trits(state_trits, MAM2_SPONGE_WIDTH, s->state,
                      MAM2_SPONGE_WIDTH, MAM2_SPONGE_WIDTH);
  s->f(s->stack, state_trits);
  flex_trits_from_trits(s->state, MAM2_SPONGE_WIDTH, state_trits,
                        MAM2_SPONGE_WIDTH, MAM2_SPONGE_WIDTH);
}

MAM2_SAPI void sponge_fork(isponge *s, isponge *fork) {
  flex_trits_slice(fork->state, MAM2_SPONGE_WIDTH, s->state, MAM2_SPONGE_WIDTH,
                   0, MAM2_SPONGE_WIDTH);
}

MAM2_SAPI void sponge_init(isponge *s) {
  memset(s->state, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_SPONGE_WIDTH);
}

MAM2_SAPI void sponge_absorb(isponge *s, trit_t c2, trits_t X) {
  TRIT_ARRAY_MAKE_FROM_RAW(X_arr, X.n - X.d, &X.p[X.d]);
  sponge_absorb_arr(s, c2, &X_arr);
}

MAM2_SAPI void sponge_absorb_arr(isponge *s, trit_t c2, trit_array_p X_arr) {
  size_t ni;
  trit_t c0, c1;

  size_t pos = 0;

  TRIT_ARRAY_DECLARE(curr_x, MAM2_SPONGE_RATE);

  do {
    trit_array_slice_at_most(X_arr, &curr_x, pos, MAM2_SPONGE_RATE);
    c0 = ((X_arr->num_trits - pos) < MAM2_SPONGE_RATE) ? 0 : 1;
    ni = curr_x.num_trits;
    pos += ni;
    c1 = (pos == X_arr->num_trits) ? 1 : -1;

    if (!is_control_trit_zero(s, 1)) {
      set_control_trits(s, 1, c0, c1, c2);
      sponge_transform(s);
    }
    flex_trits_insert(s->state, MAM2_SPONGE_WIDTH, curr_x.trits, ni, 0, ni);
    pad_1_and_0s(s, ni);
    set_control_trits(s, 0, c0, c1, c2);
  } while (pos < X_arr->num_trits);
}

MAM2_SAPI void sponge_squeeze_arr(isponge *s, trit_t c2, trit_array_p Y) {
  size_t ni;
  trit_t c0 = -1, c1;

  TRIT_ARRAY_DECLARE(curr_y, MAM2_SPONGE_RATE);
  size_t pos = 0;

  do {
    trit_array_slice_at_most(Y, &curr_y, pos, MAM2_SPONGE_RATE);
    c1 = (Y->num_trits - pos < MAM2_SPONGE_RATE) ? 0 : 1;
    ni = curr_y.num_trits;
    pos += ni;

    set_control_trits(s, 1, c0, c1, c2);
    sponge_transform(s);

    flex_trits_slice(Y->trits, ni, s->state, ni, 0, ni);
    set_control_trits(s, 0, c0, c1, c2);
  } while (pos < Y->num_trits);
}

MAM2_SAPI void sponge_squeeze(isponge *s, trit_t c2, trits_t Y) {
  size_t num_trits = Y.n - Y.d;
  TRIT_ARRAY_MAKE_FROM_RAW(Y_arr, num_trits, &Y.p[Y.d]);
  sponge_squeeze_arr(s, c2, &Y_arr);
  flex_trits_to_trits(&Y.p[Y.d], num_trits, Y_arr.trits, num_trits, num_trits);
}

MAM2_SAPI void sponge_absorbn(isponge *s, trit_t c2, size_t n, trits_t *Xs) {
  buffers_t tb = buffers_init(n, Xs);
  size_t m = buffers_size(tb);
  size_t ni;
  trit_t c0, c1;

  trit_t tmp[MAM2_SPONGE_WIDTH];

  do {
    ni = (m < MAM2_SPONGE_RATE) ? m : MAM2_SPONGE_RATE;
    m -= ni;
    c0 = (ni < MAM2_SPONGE_RATE) ? 0 : 1;
    c1 = (0 == m) ? 1 : -1;

    if (!is_control_trit_zero(s, 0)) {
      set_control_trits(s, 1, c0, c1, c2);
      sponge_transform(s);
    }

    // FIXME (@tsvisabo) - remove this abomination
    flex_trits_to_trits(tmp, ni, s->state, ni, ni);
    trits_t buf = trits_from_rep(ni, tmp);
    buffers_copy_to(&tb, buf);
    flex_trits_from_trits(s->state, ni, buf.p, ni, ni);

    pad_1_and_0s(s, ni);
    set_control_trits(s, 0, c0, c1, c2);

  } while (0 < m);
}

MAM2_SAPI void sponge_encr(isponge *s, trit_array_p X, trit_array_p Y) {
  size_t ni;
  trit_t c0, c1, c2 = -1;
  size_t pos = 0;

  TRIT_ARRAY_DECLARE(curr_X, MAM2_SPONGE_RATE);
  TRIT_ARRAY_DECLARE(curr_Y, MAM2_SPONGE_RATE);
  TRIT_ARRAY_DECLARE(curr_state, MAM2_SPONGE_RATE);

  do {
    trit_array_slice_at_most(X, &curr_X, pos, MAM2_SPONGE_RATE);
    trit_array_slice_at_most(Y, &curr_Y, pos, MAM2_SPONGE_RATE);
    c0 = (X->num_trits - pos < MAM2_SPONGE_RATE) ? 0 : 1;
    ni = curr_X.num_trits;
    pos += ni;
    c1 = pos == X->num_trits ? 1 : -1;

    set_control_trits(s, 1, c0, c1, c2);

    sponge_transform(s);

    trit_array_set_trits(&curr_state, s->state, ni);
    trit_array_copy_add_xor(&curr_X, &curr_state, &curr_Y);
    trit_array_insert(Y, &curr_Y, pos - ni, ni);
    pad_1_and_0s(s, ni);
    set_control_trits(s, 0, c0, c1, c2);

  } while (pos < X->num_trits);
}

MAM2_SAPI void sponge_decr(isponge *s, trit_array_p Y, trit_array_p X) {
  size_t ni;
  trit_t c0, c1, c2 = -1;
  size_t pos = 0;

  TRIT_ARRAY_DECLARE(curr_X, MAM2_SPONGE_RATE);
  TRIT_ARRAY_DECLARE(curr_Y, MAM2_SPONGE_RATE);
  TRIT_ARRAY_DECLARE(curr_state, MAM2_SPONGE_RATE);

  do {
    trit_array_slice_at_most(X, &curr_X, pos, MAM2_SPONGE_RATE);
    trit_array_slice_at_most(Y, &curr_Y, pos, MAM2_SPONGE_RATE);
    c0 = (X->num_trits - pos < MAM2_SPONGE_RATE) ? 0 : 1;
    ni = curr_X.num_trits;
    pos += ni;
    c1 = pos == X->num_trits ? 1 : -1;

    set_control_trits(s, 1, c0, c1, c2);

    sponge_transform(s);

    trit_array_set_trits(&curr_state, s->state, ni);
    trit_array_xor_sub(&curr_Y, &curr_state, &curr_X);
    trit_array_insert(X, &curr_X, pos - ni, ni);

    pad_1_and_0s(s, ni);
    set_control_trits(s, 0, c0, c1, c2);

  } while (pos < X->num_trits);
}

MAM2_SAPI void sponge_hash(isponge *s, trits_t X, trits_t Y) {
  sponge_init(s);
  sponge_absorb(s, MAM2_SPONGE_CTL_HASH, X);
  sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, Y);
}

MAM2_SAPI void sponge_hashn(isponge *s, size_t n, trits_t *Xs, trits_t Y) {
  sponge_init(s);
  sponge_absorbn(s, MAM2_SPONGE_CTL_HASH, n, Xs);
  sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, Y);
}
