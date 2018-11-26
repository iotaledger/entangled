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
  trits_t x, state = trits_from_rep(MAM2_SPONGE_WIDTH, s);
  trits_t stack = trits_from_rep(MAM2_SPONGE_WIDTH, (trit_t *)buf);
  trit_t t, prev = trits_get1(state);
  size_t i, j = 0;
  for (; rounds--;) {
    x = state;
    trits_copy(x, stack);
    for (i = 0; i++ < MAM2_SPONGE_WIDTH;) {
      j = (j < 365) ? (j + 364) : (j - 365);
      t = trits_get1(trits_drop(stack, j));
      trits_put1(x, curl_table[prev + (t << 2) + 5]);
      x = trits_drop(x, 1);
      prev = t;
    }
  }
}

/*! \brief Curl-81 transform */
static void curl_f81(void *buf, trit_t *s) { curl_f(81, buf, s); }

/*! \brief Curl-27 transform */
static void curl_f27(void *buf, trit_t *s) { curl_f(27, buf, s); }

static trits_t curl_state_trits(curl_sponge *s) {
  return trits_from_rep(MAM2_SPONGE_WIDTH, s->s);
}

static trits_t curl_outer_trits(curl_sponge *s, size_t n) {
  MAM2_ASSERT(n <= MAM2_CURL_RATE);
  return trits_take(curl_state_trits(s), n);
}

static void curl_transform(curl_sponge *s) { s->i.f(s->i.stack, s->i.s); }

void curl_init(curl_sponge *s) { trits_set_zero(curl_state_trits(s)); }

void curl_absorb(curl_sponge *s, trits_t X) {
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

void curl_squeeze(curl_sponge *s, trits_t Y) {
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

sponge_t *curl_sponge_init(curl_sponge *c) {
  c->i.s = c->s;
  c->i.stack = c->s2;
  c->i.f = curl_f81;
  return &c->i;
}
