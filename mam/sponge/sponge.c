/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/sponge/sponge.h"
#include "common/defs.h"
#include "mam/trits/buffers.h"
#include "mam/troika/troika.h"

static trits_t sponge_state_trits(mam_sponge_t const *const sponge) {
  return trits_from_rep(MAM_SPONGE_WIDTH, sponge->state);
}

static trits_t sponge_control_trits(mam_sponge_t const *const sponge) {
  return trits_take(trits_drop(sponge_state_trits(sponge), MAM_SPONGE_RATE), MAM_SPONGE_CONTROL);
}

static void sponge_set_control012(mam_sponge_t *const sponge, trit_t const c0, trit_t const c1, trit_t const c2) {
  trits_t t = trits_take(sponge_control_trits(sponge), NUMBER_OF_TRITS_IN_A_TRYTE);

  trits_put1(t, c0);
  trits_drop(t, 1);
  trits_put1(t, c1);
  trits_drop(t, 1);
  trits_put1(t, c2);
}

static void sponge_set_control345(mam_sponge_t *const sponge, trit_t const c3, trit_t const c4, trit_t const c5) {
  trits_t t = trits_drop(sponge_control_trits(sponge), NUMBER_OF_TRITS_IN_A_TRYTE);

  trits_put1(t, c3);
  trits_drop(t, 1);
  trits_put1(t, c4);
  trits_drop(t, 1);
  trits_put1(t, c5);
}

static trit_t sponge_get_control1(mam_sponge_t const *const sponge) {
  return trits_get1(trits_drop(sponge_control_trits(sponge), 1));
}

trits_t mam_sponge_outer_trits(mam_sponge_t const *const sponge) {
  return trits_take(sponge_state_trits(sponge), MAM_SPONGE_RATE);
}

void mam_sponge_init(mam_sponge_t *const sponge) { trits_set_zero(sponge_state_trits(sponge)); }

void mam_sponge_transform(mam_sponge_t *const sponge) { mam_ftroika_transform(sponge->state, MAM_SPONGE_WIDTH); }

void mam_sponge_fork(mam_sponge_t const *const sponge, mam_sponge_t *const fork) {
  trits_copy(sponge_state_trits(sponge), sponge_state_trits(fork));
}

void mam_sponge_absorb(mam_sponge_t *const sponge, trit_t const c2, trits_t data) {
  trits_t s1;
  trits_t curr_data_part;
  size_t ni;
  trit_t c0 = 1, c1;

  MAM_ASSERT(c2 == MAM_SPONGE_CTL_DATA || c2 == MAM_SPONGE_CTL_KEY);

  s1 = mam_sponge_outer_trits(sponge);

  do {
    curr_data_part = trits_take_min(data, MAM_SPONGE_RATE);
    ni = trits_size(curr_data_part);
    data = trits_drop(data, ni);
    c0 = (ni < MAM_SPONGE_RATE) ? 0 : 1;
    c1 = trits_is_empty(data) ? -1 : 1;

    if (sponge_get_control1(sponge) != 0) {
      sponge_set_control345(sponge, c0, c1, c2);
      mam_sponge_transform(sponge);
    }

    trits_copy(curr_data_part, trits_take(s1, ni));
    trits_padc(1, trits_drop(s1, ni));
    sponge_set_control012(sponge, c0, c1, c2);
  } while (!trits_is_empty(data));
}

void mam_sponge_absorbn(mam_sponge_t *const sponge, trit_t const c2, size_t const n, trits_t const *const data_blocks) {
  buffers_t buffers;
  size_t m;
  trits_t outer_state_trits;
  size_t ni;
  trit_t c0 = 1, c1;

  MAM_ASSERT(c2 == MAM_SPONGE_CTL_DATA || c2 == MAM_SPONGE_CTL_KEY);

  buffers = buffers_init(n, data_blocks);
  m = buffers_size(buffers);
  outer_state_trits = mam_sponge_outer_trits(sponge);

  do {
    ni = (m < MAM_SPONGE_RATE) ? m : MAM_SPONGE_RATE;
    m -= ni;
    c0 = (ni < MAM_SPONGE_RATE) ? 0 : 1;
    c1 = (0 == m) ? -1 : 1;

    if (sponge_get_control1(sponge) != 0) {
      sponge_set_control345(sponge, c0, c1, c2);
      mam_sponge_transform(sponge);
    }

    buffers_copy_to(&buffers, trits_take(outer_state_trits, ni));
    trits_padc(1, trits_drop(outer_state_trits, ni));
    sponge_set_control012(sponge, c0, c1, c2);
  } while (0 < m);
}

void mam_sponge_squeeze(mam_sponge_t *const sponge, trit_t const c2, trits_t squeezed) {
  trits_t outer_state_trits;
  trits_t curr_squeezed;
  size_t ni;
  trit_t c0 = -1, c1;

  outer_state_trits = mam_sponge_outer_trits(sponge);

  do {
    curr_squeezed = trits_take_min(squeezed, MAM_SPONGE_RATE);
    ni = trits_size(curr_squeezed);
    squeezed = trits_drop(squeezed, ni);
    c1 = trits_is_empty(squeezed) ? -1 : 1;

    sponge_set_control345(sponge, c0, c1, c2);
    mam_sponge_transform(sponge);

    trits_copy(trits_take(outer_state_trits, ni), curr_squeezed);
    trits_set_zero(trits_take(outer_state_trits, ni));
    trits_padc(1, trits_drop(outer_state_trits, ni));
    sponge_set_control012(sponge, c0, c1, c2);
  } while (!trits_is_empty(squeezed));
}

void mam_sponge_encr(mam_sponge_t *const sponge, trits_t plaintext, trits_t ciphertext) {
  trits_t outer_state_trits;
  trits_t curr_plaintext_part, curr_ciphertext_part;
  size_t ni;
  trit_t c0 = 1, c1, c2 = -1;

  MAM_ASSERT(trits_size(plaintext) == trits_size(ciphertext));
  MAM_ASSERT(trits_is_same(plaintext, ciphertext) || !trits_is_overlapped(plaintext, ciphertext));

  outer_state_trits = mam_sponge_outer_trits(sponge);

  do {
    curr_plaintext_part = trits_take_min(plaintext, MAM_SPONGE_RATE);
    curr_ciphertext_part = trits_take_min(ciphertext, MAM_SPONGE_RATE);
    ni = trits_size(curr_plaintext_part);
    plaintext = trits_drop(plaintext, ni);
    ciphertext = trits_drop(ciphertext, ni);
    c0 = (ni < MAM_SPONGE_RATE) ? 0 : 1;
    c1 = trits_is_empty(plaintext) ? -1 : 1;

    sponge_set_control345(sponge, c0, c1, c2);
    mam_sponge_transform(sponge);

    if (trits_is_same(curr_plaintext_part, curr_ciphertext_part)) {
      trits_swap_add(curr_plaintext_part, trits_take(outer_state_trits, ni));
    } else {
      trits_copy_add(curr_plaintext_part, trits_take(outer_state_trits, ni), curr_ciphertext_part);
    }

    trits_padc(1, trits_drop(outer_state_trits, ni));
    sponge_set_control012(sponge, c0, c1, c2);
  } while (!trits_is_empty(plaintext));
}

void mam_sponge_decr(mam_sponge_t *const sponge, trits_t ciphertext, trits_t plaintext) {
  trits_t outer_state_trits;
  trits_t curr_plainttext_part, curr_ciphertext_part;
  size_t ni;
  trit_t c0 = 1, c1, c2 = -1;

  MAM_ASSERT(trits_size(plaintext) == trits_size(ciphertext));
  MAM_ASSERT(trits_is_same(plaintext, ciphertext) || !trits_is_overlapped(plaintext, ciphertext));

  outer_state_trits = mam_sponge_outer_trits(sponge);

  do {
    curr_plainttext_part = trits_take_min(plaintext, MAM_SPONGE_RATE);
    curr_ciphertext_part = trits_take_min(ciphertext, MAM_SPONGE_RATE);
    ni = trits_size(curr_plainttext_part);
    plaintext = trits_drop(plaintext, ni);
    ciphertext = trits_drop(ciphertext, ni);
    c0 = (ni < MAM_SPONGE_RATE) ? 0 : 1;
    c1 = trits_is_empty(plaintext) ? -1 : 1;

    sponge_set_control345(sponge, c0, c1, c2);
    mam_sponge_transform(sponge);

    if (trits_is_same(curr_plainttext_part, curr_ciphertext_part)) {
      trits_swap_sub(curr_ciphertext_part, trits_take(outer_state_trits, ni));
    } else {
      trits_copy_sub(curr_ciphertext_part, trits_take(outer_state_trits, ni), curr_plainttext_part);
    }
    trits_padc(1, trits_drop(outer_state_trits, ni));
    sponge_set_control012(sponge, c0, c1, c2);
  } while (!trits_is_empty(ciphertext));
}

void mam_sponge_hash(mam_sponge_t *const sponge, trits_t const plaintext, trits_t digest) {
  mam_sponge_init(sponge);
  mam_sponge_absorb(sponge, MAM_SPONGE_CTL_DATA, plaintext);
  mam_sponge_squeeze(sponge, MAM_SPONGE_CTL_HASH, digest);
}

void mam_sponge_hashn(mam_sponge_t *const sponge, size_t const n, trits_t const *const plaintext_blocks,
                      trits_t digest) {
  mam_sponge_init(sponge);
  mam_sponge_absorbn(sponge, MAM_SPONGE_CTL_DATA, n, plaintext_blocks);
  mam_sponge_squeeze(sponge, MAM_SPONGE_CTL_HASH, digest);
}
