/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "mam/sponge/sponge.h"
#include "mam/sponge/spongos.h"

static trits_t spongos_outer_trits(mam_spongos_t *const spongos) {
  return trits_drop(mam_sponge_outer_trits(&spongos->sponge), spongos->pos);
}

static void spongos_update(mam_spongos_t *const spongos, size_t const n) {
  spongos->pos += n;
  if (spongos->pos == MAM_SPONGE_RATE) {
    mam_spongos_commit(spongos);
  }
}

void mam_spongos_init(mam_spongos_t *const spongos) {
  mam_sponge_init(&spongos->sponge);
  spongos->pos = 0;
}

void mam_mam_spongos_fork(mam_spongos_t const *const spongos, mam_spongos_t *const fork) {
  mam_sponge_fork(&spongos->sponge, &fork->sponge);
  fork->pos = spongos->pos;
}

void mam_spongos_commit(mam_spongos_t *const spongos) {
  if (spongos->pos != 0) {
    mam_sponge_transform(&spongos->sponge);
    spongos->pos = 0;
  }
}

void mam_spongos_absorb(mam_spongos_t *const spongos, trits_t input) {
  for (size_t n = 0; !trits_is_empty(input); input = trits_drop(input, n)) {
    n = trits_copy_min(input, spongos_outer_trits(spongos));
    spongos_update(spongos, n);
  }
}

void mam_spongos_absorbn(mam_spongos_t *const spongos, size_t const n, trits_t *const inputs) {
  for (size_t i = 0; i < n; i++) {
    mam_spongos_absorb(spongos, inputs[i]);
  }
}

void mam_spongos_squeeze(mam_spongos_t *const spongos, trits_t output) {
  for (size_t n = 0; !trits_is_empty(output); output = trits_drop(output, n)) {
    n = trits_copy_min(spongos_outer_trits(spongos), output);
    trits_set_zero(trits_take(spongos_outer_trits(spongos), n));
    spongos_update(spongos, n);
  }
}

bool mam_spongos_squeeze_eq(mam_spongos_t *const spongos, trits_t expected_output) {
  bool r = true;
  trits_t y;

  for (size_t n = 0; !trits_is_empty(expected_output); expected_output = trits_drop(expected_output, n)) {
    y = trits_take_min(spongos_outer_trits(spongos), trits_size(expected_output));
    n = trits_size(y);
    r = trits_cmp_eq(y, trits_take(expected_output, n)) && r;
    trits_set_zero(trits_take(spongos_outer_trits(spongos), n));
    spongos_update(spongos, n);
  }

  return r;
}

void mam_spongos_hash(mam_spongos_t *const spongos, trits_t input, trits_t output) {
  mam_spongos_init(spongos);
  mam_spongos_absorb(spongos, input);
  mam_spongos_commit(spongos);
  mam_spongos_squeeze(spongos, output);
}

void mam_spongos_hashn(mam_spongos_t *const spongos, size_t const n, trits_t *inputs, trits_t output) {
  mam_spongos_init(spongos);
  mam_spongos_absorbn(spongos, n, inputs);
  mam_spongos_commit(spongos);
  mam_spongos_squeeze(spongos, output);
}

void mam_spongos_encr(mam_spongos_t *const spongos, trits_t plaintext, trits_t ciphertext) {
  for (size_t n = 0; !trits_is_empty(ciphertext);
       ciphertext = trits_drop(ciphertext, n), plaintext = trits_drop(plaintext, n)) {
    if (trits_is_same(plaintext, ciphertext))
      n = trits_swap_add_min(plaintext, spongos_outer_trits(spongos));
    else
      n = trits_copy_add_min(plaintext, spongos_outer_trits(spongos), ciphertext);
    spongos_update(spongos, n);
  }
}

void mam_spongos_decr(mam_spongos_t *const spongos, trits_t ciphertext, trits_t plaintext) {
  for (size_t n = 0; !trits_is_empty(plaintext);
       plaintext = trits_drop(plaintext, n), ciphertext = trits_drop(ciphertext, n)) {
    if (trits_is_same(ciphertext, plaintext))
      n = trits_swap_sub_min(ciphertext, spongos_outer_trits(spongos));
    else
      n = trits_copy_sub_min(ciphertext, spongos_outer_trits(spongos), plaintext);
    spongos_update(spongos, n);
  }
}