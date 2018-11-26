/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/buffers.h"
#include "mam/v2/curl.h"
#include "mam/v2/mam.h"
#include "mam/v2/mss.h"
#include "mam/v2/ntru.h"
#include "mam/v2/pb3.h"
#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"
#include "mam/v2/tests/common.h"
#include "mam/v2/trits.h"
#include "mam/v2/wots.h"

#include <string.h>
#include <unity/unity.h>

#include <memory.h>
#include <stdio.h>

static sponge_t *test_create_sponge() {
  test_sponge_t *t = malloc(sizeof(test_sponge_t));
  return test_sponge_init(t);
}

static void test_delete_sponge(sponge_t *s) { free((test_sponge_t *)s); }

void mam_test_do(sponge_t *s, void *sponge_alloc_ctx,
                 sponge_t *(create_sponge)(void *ctx),
                 void (*destroy_sponge)(void *ctx, sponge_t *), prng_t *pa,
                 prng_t *pb) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;

#if 0
    struct
  {
    mam2_send_msg_context cfg[1];
    MAM2_TRITS_DECL(ch_name, 30);
    trits_t ch_name;
    MAM2_TRITS_DECL(ep_name, 30);
    trits_t ep_name;
  } a;
  struct
  {
    mam2_recv_msg_context cfg[1];
    MAM2_TRITS_DECL(ch_name, 30);
    trits_t ch_name;
    MAM2_TRITS_DECL(ep_name, 30);
    trits_t ep_name;
  } b;

  MAM2_TRITS_DEF(paK, MAM2_PRNG_KEY_SIZE);
  MAM2_TRITS_DEF(pbK, MAM2_PRNG_KEY_SIZE);

  memset(&a, 0, sizeof(a));
  trits_from_str(paK,
    "999999999999999999999999999"
    "999999999999999999999999999"
    "999999999999999999999999999"
    );
  prng_init(pa, paK);
  a.st->sponge_alloc_ctx = sponge_alloc_ctx;
  a.st->create_sponge = create_sponge;
  a.st->destroy_sponge = destroy_sponge;
  a.cfg->prng = pa;
  a.cfg->s = s;
  a.cfg->rng = pa;
  a.ch_name = MAM2_TRITS_INIT(a.ch_name, 30);
  a.ep_name = MAM2_TRITS_INIT(a.ep_name, 30);

  memset(&b, 0, sizeof(b));
  trits_from_str(pbK,
    "A99999999999999999999999999"
    "999999999999999999999999999"
    "999999999999999999999999999"
    );
  prng_init(pb, pbK);
  b.st->sponge_alloc_ctx = sponge_alloc_ctx;
  b.st->create_sponge = create_sponge;
  b.st->destroy_sponge = destroy_sponge;
  b.st->prng = pb;
  b.cfg->s = s;
  b.cfg->rng = pb;
  b.ch_name = MAM2_TRITS_INIT(b.ch_name, 30);
  b.ep_name = MAM2_TRITS_INIT(b.ep_name, 30);

  MAM2_TRITS_DEF(payload, 30);
  mam2_channel_node *chn = 0;
  mam2_endpoint_node *epn = 0;
  trits_t msg = trits_null(), payload2 = trits_null();

  do {
    trits_from_str(a.ch_name, "ABCDEFGHIJ");
    err_bind(mam2_create_channel(a.st, 2, a.ch_name, &chn));

    trits_from_str(a.ep_name, "9BCDEFGHIJ");
    err_bind(mam2_create_endpoint(a.st, 2, a.ch_name, a.ep_name, &epn));

    a.cfg->ch = &chn->info;
    a.cfg->ep = &epn->info;
    err_bind(mam2_send(a.st, a.cfg, payload, &msg));
    err_bind(mam2_recv_msg(b.st, b.cfg, msg, &payload2));

    mam2_delete_channel(a.st, chn);
    mam2_delete_endpoint(a.st, epn);
  } while(0);
#endif
}

void mam_test() {
  test_sponge_t _s[1];
  test_prng_t _p[1], _pa[1], _pb[1];
  test_wots_t _w[1];
  test_mss1 _m1[1];
  test_mss2 _m2[1];
  test_mss3 _m3[1];
  test_mss4 _m4[1];
  test_mss _m[1];

  sponge_t *s = test_sponge_init(_s);
  prng_t *p = test_prng_init(_p, s);
  prng_t *pa = test_prng_init(_pa, s);
  prng_t *pb = test_prng_init(_pb, s);
  wots_t *w = test_wots_init(_w, s);
  mss_t *m1 = test_mss_init1(_m1);
  mss_t *m2 = test_mss_init2(_m2);
  mss_t *m3 = test_mss_init3(_m3);
  mss_t *m4 = test_mss_init4(_m4);
  mss_t *m = test_mss_init(_m);
  mam_test_do(s, 0, test_create_sponge, test_delete_sponge, pa, pb);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(mam_test);

  return UNITY_END();
}
