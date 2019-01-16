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

static trits_t spongos_outer_trits(spongos_t *s) {
  return trits_drop(sponge_outer_trits(s->sponge), s->pos);
}

static void spongos_update(spongos_t *s, size_t n) {
  s->pos += n;
  if (s->pos == MAM2_SPONGE_RATE) spongos_commit(s);
}

void spongos_fork(spongos_t *s, spongos_t *fork) {
  sponge_fork(s->sponge, fork->sponge);
  fork->pos = s->pos;
}

void spongos_init(spongos_t *s) {
  sponge_init(s->sponge);
  s->pos = 0;
}

void spongos_commit(spongos_t *s) {
  if (0 != s->pos) {
    sponge_transform(s->sponge);
    s->pos = 0;
  }
}

void spongos_absorb(spongos_t *s, trits_t X) {
  size_t n;
  for (n = 0; !trits_is_empty(X); X = trits_drop(X, n)) {
    n = trits_copy_min(X, spongos_outer_trits(s));
    spongos_update(s, n);
  }
}

void spongos_absorbn(spongos_t *s, size_t n, trits_t *Xs) {
  for (; n--;) spongos_absorb(s, *Xs++);
}

void spongos_squeeze(spongos_t *s, trits_t Y) {
  size_t n;
  for (n = 0; !trits_is_empty(Y); Y = trits_drop(Y, n)) {
    n = trits_copy_min(spongos_outer_trits(s), Y);
    trits_set_zero(trits_take(spongos_outer_trits(s), n));
    spongos_update(s, n);
  }
}

bool_t spongos_squeeze_eq(spongos_t *s, trits_t Y) {
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

void spongos_encr(spongos_t *s, trits_t X, trits_t Y) {
  size_t n;
  for (n = 0; !trits_is_empty(Y); Y = trits_drop(Y, n), X = trits_drop(X, n)) {
    if (trits_is_same(X, Y))
      n = trits_swap_add_min(X, spongos_outer_trits(s));
    else
      n = trits_copy_add_min(X, spongos_outer_trits(s), Y);
    spongos_update(s, n);
  }
}

void spongos_decr(spongos_t *s, trits_t X, trits_t Y) {
  size_t n;
  for (n = 0; !trits_is_empty(Y); Y = trits_drop(Y, n), X = trits_drop(X, n)) {
    if (trits_is_same(X, Y))
      n = trits_swap_sub_min(X, spongos_outer_trits(s));
    else
      n = trits_copy_sub_min(X, spongos_outer_trits(s), Y);
    spongos_update(s, n);
  }
}

void spongos_hash(spongos_t *s, trits_t X, trits_t Y) {
  spongos_init(s);
  spongos_absorb(s, X);
  spongos_commit(s);
  spongos_squeeze(s, Y);
}

void spongos_hashn(spongos_t *s, size_t n, trits_t *Xs, trits_t Y) {
  spongos_init(s);
  spongos_absorbn(s, n, Xs);
  spongos_commit(s);
  spongos_squeeze(s, Y);
}
