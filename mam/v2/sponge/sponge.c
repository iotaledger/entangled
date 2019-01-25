/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/sponge/sponge.h"
#include "common/defs.h"
#include "mam/v2/trits/buffers.h"

static trits_t sponge_state_trits(sponge_t *sponge) {
  return trits_from_rep(MAM2_SPONGE_WIDTH, sponge->state);
}

static trits_t sponge_control_trits(sponge_t *sponge) {
  return trits_take(trits_drop(sponge_state_trits(sponge), MAM2_SPONGE_RATE),
                    MAM2_SPONGE_CONTROL);
}

trits_t sponge_outer_trits(sponge_t *sponge) {
  return trits_take(sponge_state_trits(sponge), MAM2_SPONGE_RATE);
}

static void sponge_set_control12(sponge_t *sponge, trit_t c1, trit_t c2) {
  trits_t t =
      trits_take(sponge_control_trits(sponge), NUMBER_OF_TRITS_IN_A_TRYTE);
  trits_drop(t, 1);
  trits_put1(t, c1);
  trits_drop(t, 1);
  trits_put1(t, c2);
}

static void sponge_set_control012(sponge_t *sponge, trit_t c0, trit_t c1,
                                  trit_t c2) {
  trits_t t =
      trits_take(sponge_control_trits(sponge), NUMBER_OF_TRITS_IN_A_TRYTE);
  trits_put1(t, c0);
  trits_drop(t, 1);
  trits_put1(t, c1);
  trits_drop(t, 1);
  trits_put1(t, c2);
}

static void sponge_set_control345(sponge_t *sponge, trit_t c3, trit_t c4,
                                  trit_t c5) {
  trits_t t =
      trits_drop(sponge_control_trits(sponge), NUMBER_OF_TRITS_IN_A_TRYTE);
  trits_put1(t, c3);
  trits_drop(t, 1);
  trits_put1(t, c4);
  trits_drop(t, 1);
  trits_put1(t, c5);
}

static trit_t sponge_get_control1(sponge_t *sponge) {
  return trits_get1(trits_drop(sponge_control_trits(sponge), 1));
}

void sponge_transform(sponge_t *sponge) {
  sponge->f(sponge->stack, sponge->state);
}

void sponge_fork(sponge_t *sponge, sponge_t *fork) {
  trits_copy(sponge_state_trits(sponge), sponge_state_trits(fork));
}

void sponge_init(sponge_t *sponge) {
  trits_set_zero(sponge_state_trits(sponge));
}

void sponge_absorb(sponge_t *sponge, trit_t c2, trits_t data) {
  trits_t s1;
  trits_t curr_data_part;
  size_t ni;
  trit_t c0 = 1, c1;

  MAM2_ASSERT(c2 == MAM2_SPONGE_CTL_DATA || c2 == MAM2_SPONGE_CTL_KEY);

  s1 = sponge_outer_trits(sponge);

  do {
    curr_data_part = trits_take_min(data, MAM2_SPONGE_RATE);
    ni = trits_size(curr_data_part);
    data = trits_drop(data, ni);
    c0 = (ni < MAM2_SPONGE_RATE) ? 0 : 1;
    c1 = trits_is_empty(data) ? -1 : 1;

    if (0 != sponge_get_control1(sponge)) {
      sponge_set_control345(sponge, c0, c1, c2);
      sponge_transform(sponge);
    }

    trits_copy(curr_data_part, trits_take(s1, ni));
    trits_padc(1, trits_drop(s1, ni));
    sponge_set_control012(sponge, c0, c1, c2);
  } while (!trits_is_empty(data));
}

void sponge_absorbn(sponge_t *sponge, trit_t c2, size_t n,
                    trits_t const *const data_blocks) {
  buffers_t buffers;
  size_t m;
  trits_t outer_state_trits;
  size_t ni;
  trit_t c0 = 1, c1;

  MAM2_ASSERT(c2 == MAM2_SPONGE_CTL_DATA || c2 == MAM2_SPONGE_CTL_KEY);

  buffers = buffers_init(n, data_blocks);
  m = buffers_size(buffers);
  outer_state_trits = sponge_outer_trits(sponge);

  do {
    ni = (m < MAM2_SPONGE_RATE) ? m : MAM2_SPONGE_RATE;
    m -= ni;
    c0 = (ni < MAM2_SPONGE_RATE) ? 0 : 1;
    c1 = (0 == m) ? -1 : 1;

    if (0 != sponge_get_control1(sponge)) {
      sponge_set_control345(sponge, c0, c1, c2);
      sponge_transform(sponge);
    }

    buffers_copy_to(&buffers, trits_take(outer_state_trits, ni));
    trits_padc(1, trits_drop(outer_state_trits, ni));
    sponge_set_control012(sponge, c0, c1, c2);
  } while (0 < m);
}

void sponge_squeeze(sponge_t *sponge, trit_t c2, trits_t squeezed) {
  trits_t outer_state_trits;
  trits_t curr_squeezed;
  size_t ni;
  trit_t c0 = -1, c1;

  outer_state_trits = sponge_outer_trits(sponge);

  do {
    curr_squeezed = trits_take_min(squeezed, MAM2_SPONGE_RATE);
    ni = trits_size(curr_squeezed);
    squeezed = trits_drop(squeezed, ni);
    c1 = trits_is_empty(squeezed) ? -1 : 1;

    sponge_set_control345(sponge, c0, c1, c2);
    sponge_transform(sponge);

    trits_copy(trits_take(outer_state_trits, ni), curr_squeezed);
    trits_set_zero(trits_take(outer_state_trits, ni));
    trits_padc(1, trits_drop(outer_state_trits, ni));
    sponge_set_control012(sponge, c0, c1, c2);
  } while (!trits_is_empty(squeezed));
}

void sponge_encr(sponge_t *sponge, trits_t plaintext, trits_t ciphertext) {
  trits_t outer_state_trits;
  trits_t curr_plaintext_part, curr_ciphertext_part;
  size_t ni;
  trit_t c0 = 1, c1, c2 = -1;

  MAM2_ASSERT(trits_size(plaintext) == trits_size(ciphertext));
  MAM2_ASSERT(trits_is_same(plaintext, ciphertext) ||
              !trits_is_overlapped(plaintext, ciphertext));

  outer_state_trits = sponge_outer_trits(sponge);

  do {
    curr_plaintext_part = trits_take_min(plaintext, MAM2_SPONGE_RATE);
    curr_ciphertext_part = trits_take_min(ciphertext, MAM2_SPONGE_RATE);
    ni = trits_size(curr_plaintext_part);
    plaintext = trits_drop(plaintext, ni);
    ciphertext = trits_drop(ciphertext, ni);
    c0 = (ni < MAM2_SPONGE_RATE) ? 0 : 1;
    c1 = trits_is_empty(plaintext) ? -1 : 1;

    sponge_set_control345(sponge, c0, c1, c2);
    sponge_transform(sponge);

    if (trits_is_same(curr_plaintext_part, curr_ciphertext_part)) {
      trits_swap_add(curr_plaintext_part, trits_take(outer_state_trits, ni));
    }

    else {
      trits_copy_add(curr_plaintext_part, trits_take(outer_state_trits, ni),
                     curr_ciphertext_part);
    }

    trits_padc(1, trits_drop(outer_state_trits, ni));
    sponge_set_control012(sponge, c0, c1, c2);
  } while (!trits_is_empty(plaintext));
}

void sponge_decr(sponge_t *sponge, trits_t ciphertext, trits_t plaintext) {
  trits_t outer_state_trits;
  trits_t curr_plainttext_part, curr_ciphertext_part;
  size_t ni;
  trit_t c0 = 1, c1, c2 = -1;

  MAM2_ASSERT(trits_size(plaintext) == trits_size(ciphertext));
  MAM2_ASSERT(trits_is_same(plaintext, ciphertext) ||
              !trits_is_overlapped(plaintext, ciphertext));

  outer_state_trits = sponge_outer_trits(sponge);

  do {
    curr_plainttext_part = trits_take_min(plaintext, MAM2_SPONGE_RATE);
    curr_ciphertext_part = trits_take_min(ciphertext, MAM2_SPONGE_RATE);
    ni = trits_size(curr_plainttext_part);
    plaintext = trits_drop(plaintext, ni);
    ciphertext = trits_drop(ciphertext, ni);
    c0 = (ni < MAM2_SPONGE_RATE) ? 0 : 1;
    c1 = trits_is_empty(plaintext) ? -1 : 1;

    sponge_set_control345(sponge, c0, c1, c2);
    sponge_transform(sponge);

    if (trits_is_same(curr_plainttext_part, curr_ciphertext_part)) {
      trits_swap_sub(curr_ciphertext_part, trits_take(outer_state_trits, ni));
    } else {
      trits_copy_sub(curr_ciphertext_part, trits_take(outer_state_trits, ni),
                     curr_plainttext_part);
    }
    trits_padc(1, trits_drop(outer_state_trits, ni));
    sponge_set_control012(sponge, c0, c1, c2);
  } while (!trits_is_empty(ciphertext));
}

void sponge_hash(sponge_t *sponge, trits_t plaintext, trits_t digest) {
  sponge_init(sponge);
  sponge_absorb(sponge, MAM2_SPONGE_CTL_DATA, plaintext);
  sponge_squeeze(sponge, MAM2_SPONGE_CTL_HASH, digest);
}

void sponge_hashn(sponge_t *sponge, size_t n, trits_t *plaintext_blocks,
                  trits_t digest) {
  sponge_init(sponge);
  sponge_absorbn(sponge, MAM2_SPONGE_CTL_DATA, n, plaintext_blocks);
  sponge_squeeze(sponge, MAM2_SPONGE_CTL_HASH, digest);
}
