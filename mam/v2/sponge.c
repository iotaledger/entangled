
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

MAM2_INLINE static trits_t sponge_state_trits(isponge *s) {
  return trits_from_rep(MAM2_SPONGE_WIDTH, s->s);
}

MAM2_INLINE static trits_t sponge_outer1_trits(isponge *s) {
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

MAM2_INLINE static trits_t sponge_control_trits(isponge *s) {
  return trits_take(trits_drop(sponge_state_trits(s), MAM2_SPONGE_RATE),
                    MAM2_SPONGE_CONTROL);
}

MAM2_INLINE MAM2_SAPI trits_t sponge_outer_trits(isponge *s, size_t n) {
  MAM2_ASSERT(n <= MAM2_SPONGE_RATE);
  return trits_take(sponge_state_trits(s), n);
}

#define MAM2_SPONGE_IS_CONTROL_TRIT_ZERO(S, i) \
  (trits_get1(trits_drop(sponge_state_trits(S), MAM2_SPONGE_RATE + i)) == 0)

static void sponge_transform(isponge *s) { s->f(s->stack, s->s); }

MAM2_SAPI void sponge_fork(isponge *s, isponge *fork) {
  trits_copy(sponge_state_trits(s), sponge_state_trits(fork));
}

MAM2_SAPI void sponge_init(isponge *s) {
  trits_set_zero(sponge_state_trits(s));
}

MAM2_SAPI void sponge_absorb(isponge *s, trit_t c2, trits_t X) {
  trits_t Xi;
  size_t ni;
  trit_t c0, c1;
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

MAM2_SAPI void sponge_squeeze(isponge *s, trit_t c2, trits_t Y) {
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

MAM2_SAPI void sponge_absorbn(isponge *s, trit_t c2, size_t n, trits_t *Xs) {
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

MAM2_SAPI void sponge_encr(isponge *s, trits_t X, trits_t Y) {
  trits_t Xi, Yi;
  size_t ni;
  trit_t c0, c1, c2 = -1;
  MAM2_ASSERT(trits_size(X) == trits_size(Y));

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
    if (trits_is_same(Xi, Yi))
      trits_swap_add(Xi, sponge_outer_trits(s, ni));
    else
      trits_copy_add(Xi, sponge_outer_trits(s, ni), Yi);
    trits_pad10(trits_drop(sponge_outer1_trits(s), ni));
    set_control_tryte(s, 0, c0, c1, c2);
  } while (!trits_is_empty(X));
}

MAM2_SAPI void sponge_decr(isponge *s, trits_t Y, trits_t X) {
  trits_t Xi, Yi;
  size_t ni;
  trit_t c0, c1, c2 = -1;
  MAM2_ASSERT(trits_size(X) == trits_size(Y));

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
    if (trits_is_same(Xi, Yi))
      trits_swap_sub(Xi, sponge_outer_trits(s, ni));
    else
      trits_copy_sub(Yi, sponge_outer_trits(s, ni), Xi);
    trits_pad10(trits_drop(sponge_outer1_trits(s), ni));
    set_control_tryte(s, 0, c0, c1, c2);
  } while (!trits_is_empty(Y));
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
