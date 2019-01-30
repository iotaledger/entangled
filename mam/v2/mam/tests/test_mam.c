/*
 * Copyright (c) 2018 IOTA Stiftung
 * https:github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>
#include <unity/unity.h>

#include "mam/v2/mam/mam.h"
#include "mam/v2/test_utils/test_utils.h"

#define TEST_CHANNEL_NAME "CHANAME"
#define TEST_CHANNEL_1_NAME "CHANAME9"
#define TEST_END_POINT_NAME "EPANAME"
#define TEST_END_POINT_1_NAME "EPANAME9"
#define TEST_PRE_SHARED_KEY_A_STR "PSKIDAPSKIDAPSKIDAPSKIDAPSK"
#define TEST_PRE_SHARED_KEY_A_NONCE_STR "PSKANONCE"
#define TEST_PRE_SHARED_KEY_B_STR "PSKIDBPSKIDBPSKIDBPSKIDBPSK"
#define TEST_PRE_SHARED_KEY_B_NONCE_STR "PSKBNONCE"
#define TEST_NTRU_NONCE "NTRUBNONCE"
#define TEST_PRNG_A_KEY                                                        \
  "SENDERPRNGKEYASENDERPRNGKEYSENDERPRNGKEYASENDERPRNGKEYSENDERPRNGKEYASENDER" \
  "PRNGKEY"
#define TEST_PRNG_B_KEY                                                        \
  "RECIPIPRNGKEYBRECIPIPRNGKEYRECIPIPRNGKEYBRECIPIPRNGKEYRECIPIPRNGKEYBRECIPI" \
  "PRNGKEY"

#define TEST_MSS_DEPTH 1

// TODO - Test functions should take set of prng_t instead of raw ptrs

static trits_t mam_test_generic_send_msg(
    void *sponge_alloc_ctx, sponge_t *(create_sponge)(),
    void (*destroy_sponge)(sponge_t *), prng_t *prng_a, prng_t *prng_b,
    mam_msg_pubkey_t pubkey, mam_msg_keyload_t keyload,
    mam_msg_checksum_t checksum, mam_channel_t *const cha,
    mam_endpoint_t *const epa, mam_channel_t *const ch1a,
    mam_endpoint_t *const ep1a, mam_send_msg_context_t *const cfg_msga) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;

  trits_t msg = trits_null();
  mam_pre_shared_key_t pska[1], pskb[1];
  ntru_t ntru[1];
  mam_ntru_pk_t ntru_pk[1];
  mam_ialloc_t ma[1];
  sponge_t *sponge_send = 0, *fork_sponge_send = 0, *ntru_sponge_send = 0;

  /* init alloc */
  {
    ma->create_sponge = create_sponge;
    ma->destroy_sponge = destroy_sponge;
  }

  {
    fork_sponge_send = ma->create_sponge();
    ntru_sponge_send = ma->create_sponge();
    sponge_send = ma->create_sponge();
  }

  /* gen psk */
  {
    trits_from_str(mam_psk_id(pska), TEST_PRE_SHARED_KEY_A_STR);
    prng_gen_str(prng_a, MAM2_PRNG_DST_SEC_KEY, TEST_PRE_SHARED_KEY_A_NONCE_STR,
                 mam_psk_trits(pska));

    trits_from_str(mam_psk_id(pskb), TEST_PRE_SHARED_KEY_B_STR);
    prng_gen_str(prng_b, MAM2_PRNG_DST_SEC_KEY, TEST_PRE_SHARED_KEY_B_NONCE_STR,
                 mam_psk_trits(pskb));
  }
  /* gen recipient'spongos ntru keys */
  {
    MAM2_TRITS_DEF0(ntru_nonce, 30);
    ntru_nonce = MAM2_TRITS_INIT(ntru_nonce, 30);
    trits_from_str(ntru_nonce, TEST_NTRU_NONCE);

    e = ntru_create(ntru);
    ntru_gen(ntru, prng_b, ntru_nonce, mam_ntru_pk_trits(ntru_pk));
    TEST_ASSERT(RC_OK == e);
  }
  {
    mam_send_msg_context_t *cfg = cfg_msga;

    cfg->spongos->sponge = sponge_send;
    cfg->fork->sponge = fork_sponge_send;
    cfg->prng = prng_a;
    cfg->rng = prng_a;
    cfg->spongos_ntru->sponge = ntru_sponge_send;

    cfg->ch = cha;
    cfg->ch1 = NULL;
    cfg->ep = NULL;
    cfg->ep1 = NULL;
    if (mam_msg_pubkey_epid == pubkey) {
      cfg->ep = epa;
    } else if (mam_msg_pubkey_chid1 == pubkey) {
      cfg->ch1 = ch1a;
    } else if (mam_msg_pubkey_epid1 == pubkey) {
      cfg->ep1 = ep1a;
    }

    cfg->key_plain = 0;
    cfg->pre_shared_keys = NULL;
    cfg->ntru_public_keys = NULL;
    if (mam_msg_keyload_plain == keyload)
      cfg->key_plain = 1;
    else if (mam_msg_keyload_psk == keyload) {
      mam_pre_shared_key_t_set_add(&cfg->pre_shared_keys, pska);
      mam_pre_shared_key_t_set_add(&cfg->pre_shared_keys, pskb);
    } else if (mam_msg_keyload_ntru == keyload) {
      mam_ntru_pk_t_set_add(&cfg->ntru_public_keys, ntru_pk);
    }

    trits_from_str(mam_send_msg_cfg_nonce(cfg), "SENDERNONCEAAAAASENDERNONCE");
  }

  size_t sz;
  sz = mam_send_msg_size(cfg_msga);
  msg = trits_alloc(sz);
  TEST_ASSERT(!trits_is_null(msg));

  mam_send_msg(cfg_msga, &msg);
  TEST_ASSERT(trits_is_empty(msg));
  msg = trits_pickup(msg, sz);

  ntru_destroy(ntru);
  mam_pre_shared_key_t_set_free(&cfg_msga->pre_shared_keys);
  mam_ntru_pk_t_set_free(&cfg_msga->ntru_public_keys);

  return msg;
}

static trits_t mam_test_generic_send_first_packet(
    mam_msg_pubkey_t pubkey, mam_msg_keyload_t keyload,
    mam_msg_checksum_t checksum, mam_channel_t *const cha,
    mam_endpoint_t *const epa, mam_channel_t *const ch1a,
    mam_endpoint_t *const ep1a,
    mam_send_msg_context_t const *const cfg_msg_send, char const *payload_str) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;

  trits_t packet = trits_null(), payload = trits_null();
  mam_send_packet_context_t cfg_packet_send[1];

  /* init send packet context */
  {
    mam_send_packet_context_t *cfg = cfg_packet_send;

    cfg->spongos->sponge = cfg_msg_send->spongos->sponge;
    cfg->spongos->pos = cfg_msg_send->spongos->pos;
    cfg->ord = 0;
    cfg->checksum = checksum;
    cfg->mss = 0;
    if (mam_msg_checksum_mssig == cfg->checksum) {
      if (mam_msg_pubkey_chid == pubkey) {
        cfg->mss = &cha->mss;
      } else if (mam_msg_pubkey_epid == pubkey) {
        cfg->mss = epa->m;
      } else if (mam_msg_pubkey_chid1 == pubkey) {
        cfg->mss = &ch1a->mss;
      } else if (mam_msg_pubkey_epid1 == pubkey) {
        cfg->mss = ep1a->m;
      }
    }
  }

  size_t sz;
  payload = trits_alloc(3 * strlen(payload_str));
  TEST_ASSERT(!trits_is_null(payload));
  trits_from_str(payload, payload_str);

  sz = mam_send_packet_size(cfg_packet_send, trits_size(payload));
  packet = trits_alloc(sz);
  TEST_ASSERT(!trits_is_null(packet));

  mam_send_packet(cfg_packet_send, payload, &packet);
  TEST_ASSERT(trits_is_empty(packet));
  packet = trits_pickup(packet, sz);
  trits_set_zero(payload);
  trits_free(payload);

  return packet;
}

static void mam_test_generic_receive_msg(
    void *sponge_alloc_ctx, sponge_t *(create_sponge)(),
    void (*destroy_sponge)(sponge_t *), prng_t *prng, mam_channel_t *const cha,
    trits_t *const msg, mam_recv_msg_context_t *const cfg_msg_recv) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;

  mam_ialloc_t ma[1];
  mam_pre_shared_key_t pre_shared_key[1];
  mam_ntru_pk_t ntru_pk[1];
  ntru_t ntru[1];

  sponge_t *sponge_recv = 0, *fork_sponge_recv = 0, *mss_sponge_recv = 0,
           *wots_sponge_recv = 0, *ntru_sponge_recv = 0;

  /* init alloc */
  {
    ma->create_sponge = create_sponge;
    ma->destroy_sponge = destroy_sponge;
  }
  /* create spongos */
  {
    mss_sponge_recv = ma->create_sponge();
    wots_sponge_recv = ma->create_sponge();
    ntru_sponge_recv = ma->create_sponge();
    sponge_recv = ma->create_sponge();
    fork_sponge_recv = ma->create_sponge();
  }

  /* gen psk */
  {
    trits_from_str(mam_psk_id(pre_shared_key), TEST_PRE_SHARED_KEY_B_STR);
    prng_gen_str(prng, MAM2_PRNG_DST_SEC_KEY, TEST_PRE_SHARED_KEY_B_NONCE_STR,
                 mam_psk_trits(pre_shared_key));
  }
  /* gen recipient'spongos ntru keys */
  {
    MAM2_TRITS_DEF0(ntru_nonce, 30);
    ntru_nonce = MAM2_TRITS_INIT(ntru_nonce, 30);
    trits_from_str(ntru_nonce, TEST_NTRU_NONCE);

    e = ntru_create(ntru);
    ntru_gen(ntru, prng, ntru_nonce, mam_ntru_pk_trits(ntru_pk));
    TEST_ASSERT(RC_OK == e);
  }

  /* init recv msg context */
  {
    mam_recv_msg_context_t *cfg = cfg_msg_recv;

    cfg->allocator = ma;
    cfg->pubkey = -1;
    cfg->spongos->sponge = sponge_recv;
    cfg->fork->sponge = fork_sponge_recv;
    cfg->spongos_mss->sponge = mss_sponge_recv;
    cfg->spongos_wots->sponge = wots_sponge_recv;
    cfg->spongos_ntru->sponge = ntru_sponge_recv;

    cfg->psk = pre_shared_key;
    cfg->ntru = ntru;

    trits_copy(mam_channel_id(cha), mam_recv_msg_cfg_chid(cfg));
  }

  e = mam_recv_msg(cfg_msg_recv, msg);
  TEST_ASSERT(RC_OK == e);
  TEST_ASSERT(trits_is_empty(*msg));

  ntru_destroy(cfg_msg_recv->ntru);
  cfg_msg_recv->ntru = NULL;
}

static void mam_test_generic_receive_packet(
    mam_msg_pubkey_t pubkey, mam_msg_keyload_t keyload,
    mam_msg_checksum_t checksum, mam_channel_t *const cha,
    mam_endpoint_t *const epa, mam_channel_t *const ch1a,
    mam_endpoint_t *const ep1a,
    mam_recv_msg_context_t const *const cfg_msg_recv,
    trits_t const *const packet, trits_t *const payload) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  mam_ialloc_t ma[1];
  mam_recv_packet_context_t cfg_packet_receive[1];
  /* send/recv packet */
  {
    /*trits_free(a, payload);*/ /* init recv packet context */
    {
      mam_recv_packet_context_t *cfg = cfg_packet_receive;

      cfg->allocator = ma;
      cfg->spongos->sponge = cfg_msg_recv->spongos->sponge;
      cfg->spongos->pos = cfg_msg_recv->spongos->pos;
      cfg->ord = -1;
      cfg->pk = trits_null();
      if (mam_msg_pubkey_chid == cfg_msg_recv->pubkey) {
        cfg->pk = mam_recv_msg_cfg_chid(cfg_msg_recv);
      } else if (mam_msg_pubkey_epid == cfg_msg_recv->pubkey) {
        cfg->pk = mam_recv_msg_cfg_epid(cfg_msg_recv);
      } else if (mam_msg_pubkey_chid1 == cfg_msg_recv->pubkey) {
        cfg->pk = mam_recv_msg_cfg_chid1(cfg_msg_recv);
      } else if (mam_msg_pubkey_epid1 == cfg_msg_recv->pubkey) {
        cfg->pk = mam_recv_msg_cfg_chid1(cfg_msg_recv);
      }
      cfg->spongos_mss->sponge = cfg_msg_recv->spongos_mss->sponge;
      cfg->spongos_wots->sponge = cfg_msg_recv->spongos_wots->sponge;
    }

    e = mam_recv_packet(cfg_packet_receive, packet, payload);
    TEST_ASSERT(RC_OK == e);
    TEST_ASSERT(trits_is_empty(*packet));
    TEST_ASSERT(trits_is_empty(*payload));
    *payload = trits_pickup_all(*payload);
  }
}

static void mam_test_create_channels(sponge_t *(create_sponge)(),
                                     void (*destroy_sponge)(sponge_t *),
                                     prng_t *prng, mam_channel_t **const cha,
                                     mam_channel_t **const ch1,
                                     mam_endpoint_t **const epa,
                                     mam_endpoint_t **ep1) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  mam_ialloc_t ma[1];
  mss_mt_height_t d = TEST_MSS_DEPTH;

  /* init alloc */
  {
    ma->create_sponge = create_sponge;
    ma->destroy_sponge = destroy_sponge;
  }

  /* init rng */
  {
    MAM2_TRITS_DEF0(k, MAM2_PRNG_KEY_SIZE);
    k = MAM2_TRITS_INIT(k, MAM2_PRNG_KEY_SIZE);

    trits_from_str(k, TEST_PRNG_A_KEY);
    prng_init(prng, prng->sponge, k);
  }

  /* create channels */
  {
    trits_t cha_name = trits_alloc(3 * strlen(TEST_CHANNEL_NAME));
    trits_from_str(cha_name, TEST_CHANNEL_NAME);

    *cha = malloc(sizeof(mam_channel_t));
    TEST_ASSERT(0 != *cha);
    memset(*cha, 0, sizeof(mam_channel_t));
    e = mam_channel_create(ma, prng, d, cha_name, *cha);
    TEST_ASSERT(RC_OK == e);

    /* create endpoints */
    {
      trits_t epa_name = trits_alloc(3 * strlen(TEST_END_POINT_NAME));
      trits_from_str(epa_name, TEST_END_POINT_NAME);

      *epa = malloc(sizeof(mam_endpoint_t));
      TEST_ASSERT(0 != *epa);
      memset(*epa, 0, sizeof(mam_endpoint_t));
      e = mam_endpoint_create(ma, prng, d, cha_name, epa_name, *epa);
      TEST_ASSERT(RC_OK == e);
      trits_free(epa_name);
    }
    {
      trits_t ep1a_name = trits_alloc(3 * strlen(TEST_END_POINT_1_NAME));
      trits_from_str(ep1a_name, TEST_END_POINT_1_NAME);

      *ep1 = malloc(sizeof(mam_endpoint_t));
      TEST_ASSERT(0 != *ep1);
      memset(*ep1, 0, sizeof(mam_endpoint_t));
      e = mam_endpoint_create(ma, prng, d, cha_name, ep1a_name, *ep1);
      TEST_ASSERT(RC_OK == e);
      trits_free(ep1a_name);
    }
    trits_free(cha_name);
    {
      trits_t ch1a_name = trits_alloc(3 * strlen(TEST_CHANNEL_1_NAME));
      trits_from_str(ch1a_name, TEST_CHANNEL_1_NAME);

      *ch1 = malloc(sizeof(mam_channel_t));
      TEST_ASSERT(0 != *ch1);
      memset(*ch1, 0, sizeof(mam_channel_t));
      e = mam_channel_create(ma, prng, d, ch1a_name, *ch1);
      TEST_ASSERT(RC_OK == e);
      trits_free(ch1a_name);
    }
  }
}

static void mam_test_generic(sponge_t *s, void *sponge_alloc_ctx,
                             sponge_t *(create_sponge)(),
                             void (*destroy_sponge)(sponge_t *), prng_t *prng_a,
                             prng_t *prng_b) {
  retcode_t e = RC_OK;
  mam_ialloc_t ma[1];

  trits_t msg = trits_null(), packet = trits_null(), payload = trits_null();

  mss_mt_height_t d = TEST_MSS_DEPTH;
  mam_channel_t *cha = 0, *ch1a = 0;
  mam_endpoint_t *epa = 0, *ep1a = 0;

  mam_send_msg_context_t cfg_msg_send[1];
  mam_recv_msg_context_t cfg_msg_recv[1];

  mam_msg_pubkey_t pubkey;     /* chid=0, epid=1, chid1=2, epid1=3 */
  mam_msg_keyload_t keyload;   /* plain=0, psk=1, ntru=2 */
  mam_msg_checksum_t checksum; /* none=0, mac=1, mssig=2 */

  /* init alloc */
  {
    ma->create_sponge = create_sponge;
    ma->destroy_sponge = destroy_sponge;
  }

  mam_test_create_channels(create_sponge, destroy_sponge, prng_a, &cha, &ch1a,
                           &epa, &ep1a);

  char const *payload_str = "PAYLOAD9999";
  payload = trits_alloc(3 * strlen(payload_str));

  /* chid=0, epid=1, chid1=2, epid1=3*/
  for (pubkey = 0; pubkey < 4; ++pubkey) {
    /* plain=0, psk=1, ntru=2 */
    for (keyload = 0; keyload < 3; ++keyload) {
      for (checksum = 0; checksum < 3; ++checksum)
      /* none=0, mac=1, mssig=2 */
      {
        /* send msg and packet */
        {
          msg = mam_test_generic_send_msg(
              sponge_alloc_ctx, create_sponge, destroy_sponge, prng_a, prng_b,
              pubkey, keyload, checksum, cha, epa, ch1a, ep1a, cfg_msg_send);

          packet = mam_test_generic_send_first_packet(
              pubkey, keyload, checksum, cha, epa, ch1a, ep1a, cfg_msg_send,
              payload_str);
        }

        /* recv msg and packet */
        {
          mam_test_generic_receive_msg(sponge_alloc_ctx, create_sponge,
                                       destroy_sponge, prng_b, cha, &msg,
                                       cfg_msg_recv);

          mam_test_generic_receive_packet(pubkey, keyload, checksum, cha, epa,
                                          ch1a, ep1a, cfg_msg_recv, &packet,
                                          &payload);
          TEST_ASSERT(trits_cmp_eq_str(payload, payload_str));
        }

        /* cleanup */
        {
          trits_free(msg);
          trits_free(packet);
          ma->destroy_sponge(cfg_msg_send->spongos->sponge);
          ma->destroy_sponge(cfg_msg_send->fork->sponge);
          ma->destroy_sponge(cfg_msg_send->spongos_ntru->sponge);
          ma->destroy_sponge(cfg_msg_recv->spongos->sponge);
          ma->destroy_sponge(cfg_msg_recv->fork->sponge);
          ma->destroy_sponge(cfg_msg_recv->spongos_ntru->sponge);
          ma->destroy_sponge(cfg_msg_recv->spongos_wots->sponge);
          ma->destroy_sponge(cfg_msg_recv->spongos_mss->sponge);
        }
      }
    }
  }

  trits_free(payload);

  /* destroy channels/endpoints */
  {
    if (cha) mam_channel_destroy(ma, cha);
    if (ch1a) mam_channel_destroy(ma, ch1a);
    if (epa) mam_endpoint_destroy(ma, epa);
    if (ep1a) mam_endpoint_destroy(ma, ep1a);
    free(cha);
    free(epa);
    free(ch1a);
    free(ep1a);
  }

  TEST_ASSERT(e == RC_OK);
}

void mam_test() {
  test_sponge_t _s[1];
  sponge_t *s = test_sponge_init(_s);

  test_prng_t _pa[1], _pb[1];
  prng_t *pa = test_prng_init(_pa, s);
  prng_t *pb = test_prng_init(_pb, s);

  mam_test_generic(s, NULL, test_create_sponge, test_delete_sponge, pa, pb);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(mam_test);
  return UNITY_END();
}
