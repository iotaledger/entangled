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
\file spongos.c
\brief MAM2 Spongos layer.
*/
#include "mam/v2/sponge/spongos.h"
#include "mam/v2/sponge/sponge.h"

static trits_t spongos_outer_trits(ispongos *s) {
  return trits_drop(sponge_outer_trits(s->s), s->pos);
}

static void spongos_update(ispongos *s, size_t n) {
  s->pos += n;
  if (s->pos == MAM2_SPONGE_RATE) spongos_commit(s);
}

void spongos_fork(ispongos *s, ispongos *fork) {
  sponge_fork(s->s, fork->s);
  fork->pos = s->pos;
}

void spongos_init(ispongos *s) {
  sponge_init(s->s);
  s->pos = 0;
}

void spongos_commit(ispongos *s) {
  if (0 != s->pos) {
    sponge_transform(s->s);
    s->pos = 0;
  }
}

void spongos_absorb(ispongos *s, trits_t X) {
  size_t n;
  for (n = 0; !trits_is_empty(X); X = trits_drop(X, n)) {
    n = trits_copy_min(X, spongos_outer_trits(s));
    spongos_update(s, n);
  }
}

void spongos_absorbn(ispongos *s, size_t n, trits_t *Xs) {
  for (; n--;) spongos_absorb(s, *Xs++);
}

void spongos_squeeze(ispongos *s, trits_t Y) {
  size_t n;
  for (n = 0; !trits_is_empty(Y); Y = trits_drop(Y, n)) {
    n = trits_copy_min(spongos_outer_trits(s), Y);
    trits_set_zero(trits_take(spongos_outer_trits(s), n));
    spongos_update(s, n);
  }
}

bool_t spongos_squeeze_eq(ispongos *s, trits_t Y) {
  bool_t r = 1;
  size_t n;
  trits_t y;
  for (n = 0; !trits_is_empty(Y); Y = trits_drop(Y, n)) {
    y = trits_take_min(spongos_outer_trits(s), trits_size(Y));
    n = trits_size(y);
    r = trits_cmp_eq(y, trits_take(Y, n)) && r;
    trits_set_zero(trits_take(spongos_outer_trits(s), n));
    spongos_update(s, n);
  }
  return r;
}

void spongos_encr(ispongos *s, trits_t X, trits_t Y) {
  size_t n;
  for (n = 0; !trits_is_empty(Y); Y = trits_drop(Y, n), X = trits_drop(X, n)) {
    if (trits_is_same(X, Y))
      n = trits_swap_add_min(X, spongos_outer_trits(s));
    else
      n = trits_copy_add_min(X, spongos_outer_trits(s), Y);
    spongos_update(s, n);
  }
}

void spongos_decr(ispongos *s, trits_t X, trits_t Y) {
  size_t n;
  for (n = 0; !trits_is_empty(Y); Y = trits_drop(Y, n), X = trits_drop(X, n)) {
    if (trits_is_same(X, Y))
      n = trits_swap_sub_min(X, spongos_outer_trits(s));
    else
      n = trits_copy_sub_min(X, spongos_outer_trits(s), Y);
    spongos_update(s, n);
  }
}

void spongos_hash(ispongos *s, trits_t X, trits_t Y) {
  spongos_init(s);
  spongos_absorb(s, X);
  spongos_commit(s);
  spongos_squeeze(s, Y);
}

void spongos_hashn(ispongos *s, size_t n, trits_t *Xs, trits_t Y) {
  spongos_init(s);
  spongos_absorbn(s, n, Xs);
  spongos_commit(s);
  spongos_squeeze(s, Y);
}

bool_t spongos_test(ispongos *s) {
  bool_t r = 1;
  MAM2_TRITS_DEF0(x, 243);
  MAM2_TRITS_DEF0(y, 243);
  MAM2_TRITS_DEF0(z, 243);
  x = MAM2_TRITS_INIT(x, 243);
  y = MAM2_TRITS_INIT(y, 243);
  z = MAM2_TRITS_INIT(z, 243);

  trits_set_zero(x);

  spongos_init(s);
  spongos_absorb(s, x);
  spongos_squeeze(s, y);

  spongos_init(s);
  spongos_absorb(s, x);
  spongos_commit(s);
  spongos_encr(s, x, z);

  r = r && trits_cmp_eq(y, z);

  spongos_init(s);
  spongos_absorb(s, x);
  spongos_commit(s);
  spongos_decr(s, z, z);

  r = r && trits_cmp_eq(x, z);

  return r;
}
