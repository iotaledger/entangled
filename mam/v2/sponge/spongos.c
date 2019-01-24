/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/sponge/spongos.h"
#include "mam/v2/sponge/sponge.h"

static trits_t spongos_outer_trits(spongos_t *const spongos) {
  return trits_drop(sponge_outer_trits(spongos->sponge), spongos->pos);
}

static void spongos_update(spongos_t *const spongos, size_t const n) {
  spongos->pos += n;
  if (spongos->pos == MAM2_SPONGE_RATE) {
    spongos_commit(spongos);
  }
}

void spongos_init(spongos_t *const spongos) {
  sponge_init(spongos->sponge);
  spongos->pos = 0;
}

void spongos_fork(spongos_t const *const spongos, spongos_t *const fork) {
  sponge_fork(spongos->sponge, fork->sponge);
  fork->pos = spongos->pos;
}

void spongos_commit(spongos_t *const spongos) {
  if (spongos->pos != 0) {
    sponge_transform(spongos->sponge);
    spongos->pos = 0;
  }
}

void spongos_absorb(spongos_t *const spongos, trits_t input) {
  for (size_t n = 0; !trits_is_empty(input); input = trits_drop(input, n)) {
    n = trits_copy_min(input, spongos_outer_trits(spongos));
    spongos_update(spongos, n);
  }
}

void spongos_absorbn(spongos_t *const spongos, size_t const n,
                     trits_t *const inputs) {
  for (size_t i = 0; i < n; i++) {
    spongos_absorb(spongos, inputs[i]);
  }
}

void spongos_squeeze(spongos_t *const spongos, trits_t output) {
  for (size_t n = 0; !trits_is_empty(output); output = trits_drop(output, n)) {
    n = trits_copy_min(spongos_outer_trits(spongos), output);
    trits_set_zero(trits_take(spongos_outer_trits(spongos), n));
    spongos_update(spongos, n);
  }
}

bool spongos_squeeze_eq(spongos_t *const spongos, trits_t output) {
  bool r = true;
  trits_t y;

  for (size_t n = 0; !trits_is_empty(output); output = trits_drop(output, n)) {
    y = trits_take_min(spongos_outer_trits(spongos), trits_size(output));
    n = trits_size(y);
    r = trits_cmp_eq(y, trits_take(output, n)) && r;
    trits_set_zero(trits_take(spongos_outer_trits(spongos), n));
    spongos_update(spongos, n);
  }

  return r;
}

void spongos_hash(spongos_t *const spongos, trits_t input, trits_t output) {
  spongos_init(spongos);
  spongos_absorb(spongos, input);
  spongos_commit(spongos);
  spongos_squeeze(spongos, output);
}

void spongos_hashn(spongos_t *const spongos, size_t const n, trits_t *inputs,
                   trits_t output) {
  spongos_init(spongos);
  spongos_absorbn(spongos, n, inputs);
  spongos_commit(spongos);
  spongos_squeeze(spongos, output);
}

void spongos_encr(spongos_t *const spongos, trits_t input, trits_t output) {
  for (size_t n = 0; !trits_is_empty(output);
       output = trits_drop(output, n), input = trits_drop(input, n)) {
    if (trits_is_same(input, output))
      n = trits_swap_add_min(input, spongos_outer_trits(spongos));
    else
      n = trits_copy_add_min(input, spongos_outer_trits(spongos), output);
    spongos_update(spongos, n);
  }
}

void spongos_decr(spongos_t *const spongos, trits_t input, trits_t output) {
  for (size_t n = 0; !trits_is_empty(output);
       output = trits_drop(output, n), input = trits_drop(input, n)) {
    if (trits_is_same(input, output))
      n = trits_swap_sub_min(input, spongos_outer_trits(spongos));
    else
      n = trits_copy_sub_min(input, spongos_outer_trits(spongos), output);
    spongos_update(spongos, n);
  }
}
