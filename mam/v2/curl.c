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
\file curl.c
\brief CURL implementation.
*/
#include "mam/v2/curl.h"

// FIXME (@tsvisabo)
#define FLEX_SPONGE_WIDTH MAM2_SPONGE_WIDTH

static trit_t curl_table[] = {1, 0, -1, 2, 1, -1, 0, 2, -1, 1, 0};

/*! \brief Curl transform */
static void curl_f(int rounds, void *buf, trit_t *s) {
  flex_trit_t flex_trits_state[FLEX_SPONGE_WIDTH];
  flex_trits_from_trits(flex_trits_state, FLEX_SPONGE_WIDTH, s,
                        FLEX_SPONGE_WIDTH, FLEX_SPONGE_WIDTH);
  flex_trit_t flex_trits_stack[FLEX_SPONGE_WIDTH];
  flex_trits_from_trits(flex_trits_stack, FLEX_SPONGE_WIDTH, (trit_t *)buf,
                        FLEX_SPONGE_WIDTH, FLEX_SPONGE_WIDTH);
  trit_t t, prev_t = flex_trits_at(flex_trits_state, FLEX_SPONGE_WIDTH, 0);
  size_t i, j = 0;

  flex_trit_t *stack_p = flex_trits_stack;
  size_t stack_pos = 0;
  size_t state_pos = 0;
  for (; rounds--;) {
    flex_trits_slice(flex_trits_state, FLEX_SPONGE_WIDTH, flex_trits_stack,
                     FLEX_SPONGE_WIDTH, state_pos, FLEX_SPONGE_WIDTH);
    for (i = 0; i++ < MAM2_SPONGE_WIDTH;) {
      j = (j < 365) ? (j + 364) : (j - 365);
      stack_pos = j;
      t = flex_trits_at(stack_p, FLEX_SPONGE_WIDTH, stack_pos);
      flex_trits_set_at(flex_trits_state, FLEX_SPONGE_WIDTH, 0,
                        curl_table[prev_t + (t << 2) + 5]);
      state_pos++;
      prev_t = t;
    }
  }
}

/*! \brief Curl-81 transform */
static void curl_f81(void *buf, trit_t *s) { curl_f(81, buf, s); }

/*! \brief Curl-27 transform */
static void curl_f27(void *buf, trit_t *s) { curl_f(27, buf, s); }

MAM2_INLINE static trits_t curl_state_trits(curl_sponge *s) {
  return trits_from_rep(MAM2_SPONGE_WIDTH, s->s);
}

MAM2_INLINE static trits_t curl_outer_trits(curl_sponge *s, size_t n) {
  MAM2_ASSERT(n <= MAM2_CURL_RATE);
  return trits_take(curl_state_trits(s), n);
}

static void curl_transform(curl_sponge *s) {
  trit_t state_trits[MAM2_SPONGE_WIDTH];
  flex_trits_to_trits(state_trits, MAM2_SPONGE_WIDTH, s->i.state,
                      MAM2_SPONGE_WIDTH, MAM2_SPONGE_WIDTH);
  s->i.f(s->i.stack, state_trits);
  flex_trits_from_trits(s->i.state, MAM2_SPONGE_WIDTH, state_trits,
                        MAM2_SPONGE_WIDTH, MAM2_SPONGE_WIDTH);
}

MAM2_SAPI void curl_init(curl_sponge *s) {
  trits_set_zero(curl_state_trits(s));
}

MAM2_SAPI void curl_absorb(curl_sponge *s, trits_t X) {
  trits_t Xi;
  size_t ni;

  do {
    Xi = trits_take_min(X, MAM2_CURL_RATE);
    ni = trits_size(Xi);
    X = trits_drop(X, ni);

    trits_copy(Xi, curl_outer_trits(s, ni));
    curl_transform(s);
  } while (!trits_is_empty(X));
}

MAM2_SAPI void curl_squeeze(curl_sponge *s, trits_t Y) {
  trits_t Yi;
  size_t ni;

  do {
    Yi = trits_take_min(Y, MAM2_CURL_RATE);
    ni = trits_size(Yi);
    Y = trits_drop(Y, ni);

    trits_copy(curl_outer_trits(s, ni), Yi);
    curl_transform(s);
  } while (!trits_is_empty(Y));
}

MAM2_SAPI isponge *curl_sponge_init(curl_sponge *c) {
  c->i.stack = c->s2;
  c->i.f = curl_f81;
  return &c->i;
}
