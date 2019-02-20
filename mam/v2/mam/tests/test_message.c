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

#include "mam/v2/mam/message.h"
#include "mam/v2/ntru/mam_ntru_sk_t_set.h"
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

static trits_t message_test_generic_send_msg(
    mam_prng_t *prng, mam_psk_t const *const pska, mam_psk_t const *const pskb,
    mam_ntru_pk_t const *const ntru_pk, mam_msg_pubkey_t pubkey,
    mam_msg_keyload_t keyload, mam_msg_checksum_t checksum,
    mam_channel_t *const cha, mam_endpoint_t *const epa,
    mam_channel_t *const ch1a, mam_endpoint_t *const ep1a,
    mam_msg_send_context_t *const send_ctx) {
  trits_t msg = trits_null();
  mam_channel_t *ch = cha;
  mam_endpoint_t *ep = NULL;
  mam_channel_t *ch1 = NULL;
  mam_endpoint_t *ep1 = NULL;
  MAM2_TRITS_DEF0(msg_id, MAM2_MSG_ID_SIZE);
  msg_id = MAM2_TRITS_INIT(msg_id, MAM2_MSG_ID_SIZE);
  trits_from_str(msg_id, "SENDERMSGIDAAAAASENDERMSGID");
  trint9_t msg_type_id = 0;
  mam_psk_t_set_t psks = NULL;
  mam_ntru_pk_t_set_t ntru_pks = NULL;

  if (mam_msg_pubkey_epid == pubkey) {
    ep = epa;
  } else if (mam_msg_pubkey_chid1 == pubkey) {
    ch1 = ch1a;
  } else if (mam_msg_pubkey_epid1 == pubkey) {
    ep1 = ep1a;
  }

  if (mam_msg_keyload_psk == keyload) {
    mam_psk_t_set_add(&psks, pska);
    mam_psk_t_set_add(&psks, pskb);
  } else if (mam_msg_keyload_ntru == keyload) {
    mam_ntru_pk_t_set_add(&ntru_pks, ntru_pk);
  }

  size_t sz;
  sz = mam_msg_send_size(ch, ep, ch1, ep1, psks, ntru_pks);
  msg = trits_alloc(sz);
  TEST_ASSERT(!trits_is_null(msg));

  mam_msg_send(send_ctx, prng, ch, ep, ch1, ep1, msg_id, msg_type_id, psks,
               ntru_pks, &msg);
  TEST_ASSERT(trits_is_empty(msg));
  msg = trits_pickup(msg, sz);
  mam_psk_t_set_free(&psks);
  mam_ntru_pk_t_set_free(&ntru_pks);

  return msg;
}

static trits_t message_test_generic_send_first_packet(
    mam_msg_pubkey_t pubkey, mam_msg_checksum_t checksum,
    mam_channel_t *const cha, mam_endpoint_t *const epa,
    mam_channel_t *const ch1a, mam_endpoint_t *const ep1a,
    mam_msg_send_context_t *const send_ctx, char const *payload_str) {
  trits_t packet = trits_null();
  trits_t payload = trits_null();

  // mam_spongos_copy(&send_ctx->spongos, cfg->spongos);
  if (mam_msg_checksum_mssig == checksum) {
    if (mam_msg_pubkey_chid == pubkey) {
      send_ctx->mss = &cha->mss;
    } else if (mam_msg_pubkey_epid == pubkey) {
      send_ctx->mss = &epa->mss;
    } else if (mam_msg_pubkey_chid1 == pubkey) {
      send_ctx->mss = &ch1a->mss;
    } else if (mam_msg_pubkey_epid1 == pubkey) {
      send_ctx->mss = &ep1a->mss;
    }
  }

  size_t sz;
  payload = trits_alloc(3 * strlen(payload_str));
  TEST_ASSERT(!trits_is_null(payload));
  trits_from_str(payload, payload_str);

  sz = mam_msg_send_packet_size(checksum, send_ctx->mss, trits_size(payload));
  packet = trits_alloc(sz);
  TEST_ASSERT(!trits_is_null(packet));

  mam_msg_send_packet(send_ctx, checksum, payload, &packet);
  TEST_ASSERT(trits_is_empty(packet));
  packet = trits_pickup(packet, sz);
  trits_set_zero(payload);
  trits_free(payload);

  return packet;
}

static void message_test_generic_receive_msg(
    mam_psk_t const *const pre_shared_key, mam_ntru_sk_t const *const ntru,
    mam_channel_t *const cha, trits_t *const msg,
    mam_msg_recv_context_t *const cfg_msg_recv) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;

  /* init recv msg context */
  {
    mam_msg_recv_context_t *cfg = cfg_msg_recv;

    cfg->psks = NULL;
    TEST_ASSERT(mam_psk_t_set_add(&cfg->psks, pre_shared_key) == RC_OK);
    cfg->ntrus = NULL;
    TEST_ASSERT(mam_ntru_sk_t_set_add(&cfg->ntrus, ntru) == RC_OK);

    trits_copy(mam_channel_id(cha),
               trits_from_rep(MAM2_CHANNEL_ID_SIZE, cfg->pk));
  }

  e = mam_msg_recv(cfg_msg_recv, msg);

  TEST_ASSERT(RC_OK == e);
  TEST_ASSERT(trits_is_empty(*msg));
  MAM2_ASSERT(trits_cmp_eq_str(mam_msg_recv_cfg_msg_id(cfg_msg_recv),
                               "SENDERMSGIDAAAAASENDERMSGID"));
  MAM2_ASSERT(cfg_msg_recv->msg_type_id == 0);

  mam_ntru_sk_t_set_free(&cfg_msg_recv->ntrus);
  mam_psk_t_set_free(&cfg_msg_recv->psks);
}

static void message_test_generic_receive_packet(
    mam_msg_recv_context_t const *const cfg_msg_recv,
    trits_t const *const packet, trits_t *const payload) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  mam_msg_recv_packet_context_t cfg_packet_receive[1];
  /* send/recv packet */
  {
    /*trits_free(a, payload);*/ /* init recv packet context */
    {
      mam_msg_recv_packet_context_t *cfg = cfg_packet_receive;
      mam_spongos_copy(&cfg_msg_recv->spongos, &cfg->spongos);
      cfg->ord = -1;
      cfg->pk = trits_from_rep(MAM2_CHANNEL_ID_SIZE, cfg_msg_recv->pk);
      mam_spongos_copy(cfg_msg_recv->spongos_mss, cfg->spongos_mss);
      mam_spongos_copy(cfg_msg_recv->spongos_wots, cfg->spongos_wots);
    }

    e = mam_msg_recv_packet(cfg_packet_receive, packet, payload);
    TEST_ASSERT(RC_OK == e);
    TEST_ASSERT(trits_is_empty(*packet));
    TEST_ASSERT(trits_is_empty(*payload));
    *payload = trits_pickup_all(*payload);
  }
}

static void message_test_create_channels(mam_prng_t *prng,
                                         mam_channel_t **const cha,
                                         mam_channel_t **const ch1,
                                         mam_endpoint_t **const epa,
                                         mam_endpoint_t **ep1) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  mss_mt_height_t d = TEST_MSS_DEPTH;

  /* create channels */
  {
    trits_t cha_name = trits_alloc(3 * strlen(TEST_CHANNEL_NAME));
    trits_from_str(cha_name, TEST_CHANNEL_NAME);

    *cha = malloc(sizeof(mam_channel_t));
    TEST_ASSERT(0 != *cha);
    memset(*cha, 0, sizeof(mam_channel_t));
    e = mam_channel_create(prng, d, cha_name, *cha);
    TEST_ASSERT(RC_OK == e);

    /* create endpoints */
    {
      trits_t epa_name = trits_alloc(3 * strlen(TEST_END_POINT_NAME));
      trits_from_str(epa_name, TEST_END_POINT_NAME);

      *epa = malloc(sizeof(mam_endpoint_t));
      TEST_ASSERT(0 != *epa);
      memset(*epa, 0, sizeof(mam_endpoint_t));
      e = mam_endpoint_create(prng, d, (*cha)->name, epa_name, *epa);
      TEST_ASSERT(RC_OK == e);
      trits_free(epa_name);
    }
    {
      trits_t ep1a_name = trits_alloc(3 * strlen(TEST_END_POINT_1_NAME));
      trits_from_str(ep1a_name, TEST_END_POINT_1_NAME);

      *ep1 = malloc(sizeof(mam_endpoint_t));
      TEST_ASSERT(0 != *ep1);
      memset(*ep1, 0, sizeof(mam_endpoint_t));
      e = mam_endpoint_create(prng, d, (*cha)->name, ep1a_name, *ep1);
      TEST_ASSERT(RC_OK == e);
      trits_free(ep1a_name);
    }
    {
      trits_t ch1a_name = trits_alloc(3 * strlen(TEST_CHANNEL_1_NAME));
      trits_from_str(ch1a_name, TEST_CHANNEL_1_NAME);

      *ch1 = malloc(sizeof(mam_channel_t));
      TEST_ASSERT(0 != *ch1);
      memset(*ch1, 0, sizeof(mam_channel_t));
      e = mam_channel_create(prng, d, ch1a_name, *ch1);
      TEST_ASSERT(RC_OK == e);
      trits_free(ch1a_name);
    }
    trits_free(cha_name);
  }
}

static void message_test_generic(mam_prng_t *prng_sender,
                                 mam_prng_t *prng_receiver) {
  retcode_t e = RC_OK;

  trits_t msg = trits_null(), packet = trits_null(), payload = trits_null();

  mam_channel_t *cha = NULL, *ch1a = NULL;
  mam_endpoint_t *epa = NULL, *ep1a = NULL;

  mam_msg_send_context_t send_ctx;
  mam_spongos_init(&send_ctx.spongos);
  send_ctx.ord = 0;
  send_ctx.mss = NULL;

  mam_msg_recv_context_t cfg_msg_recv[1];

  mam_msg_pubkey_t pubkey;     /* chid=0, epid=1, chid1=2, epid1=3 */
  mam_msg_keyload_t keyload;   /* psk=1, ntru=2 */
  mam_msg_checksum_t checksum; /* none=0, mac=1, mssig=2 */

  message_test_create_channels(prng_sender, &cha, &ch1a, &epa, &ep1a);

  char const *payload_str = "PAYLOAD9999";
  payload = trits_alloc(3 * strlen(payload_str));

  mam_ntru_sk_t ntru[1];

  /* gen recipient'spongos ntru keys, public key is shared with sender */
  {
    retcode_t e;
    MAM2_TRITS_DEF0(ntru_nonce, 30);
    ntru_nonce = MAM2_TRITS_INIT(ntru_nonce, 30);
    trits_from_str(ntru_nonce, TEST_NTRU_NONCE);

    e = ntru_init(ntru);
    ntru_gen(ntru, prng_receiver, ntru_nonce);
    TEST_ASSERT(RC_OK == e);
  }

  mam_psk_t pska[1], pskb[1];

  /* gen psk */
  {
    trits_from_str(mam_psk_id(pska), TEST_PRE_SHARED_KEY_A_STR);
    prng_gen_str(prng_sender, MAM2_PRNG_DST_SEC_KEY,
                 TEST_PRE_SHARED_KEY_A_NONCE_STR, mam_psk_trits(pska));

    trits_from_str(mam_psk_id(pskb), TEST_PRE_SHARED_KEY_B_STR);
    prng_gen_str(prng_receiver, MAM2_PRNG_DST_SEC_KEY,
                 TEST_PRE_SHARED_KEY_B_NONCE_STR, mam_psk_trits(pskb));
  }

  /* chid=0, epid=1, chid1=2, epid1=3*/
  for (pubkey = 0; pubkey < 4; ++pubkey) {
    /* public=0, psk=1, ntru=2 */
    for (keyload = 0; keyload < 3; ++keyload) {
      for (checksum = 0; checksum < 3; ++checksum)
      /* none=0, mac=1, mssig=2 */
      {
        /* send msg and packet */
        {
          msg = message_test_generic_send_msg(
              prng_sender, pska, pskb, &ntru->public_key, pubkey, keyload,
              checksum, cha, epa, ch1a, ep1a, &send_ctx);

          packet = message_test_generic_send_first_packet(
              pubkey, checksum, cha, epa, ch1a, ep1a, &send_ctx, payload_str);
        }

        /* recv msg and packet */
        {
          message_test_generic_receive_msg(pskb, ntru, cha, &msg, cfg_msg_recv);

          message_test_generic_receive_packet(cfg_msg_recv, &packet, &payload);
          TEST_ASSERT(trits_cmp_eq_str(payload, payload_str));
        }

        /* cleanup */
        {
          trits_free(msg);
          trits_free(packet);
        }
      }
    }
  }

  ntru_destroy(ntru);

  trits_free(payload);

  /* destroy channels/endpoints */
  {
    if (cha) mam_channel_destroy(cha);
    if (ch1a) mam_channel_destroy(ch1a);
    if (epa) mam_endpoint_destroy(epa);
    if (ep1a) mam_endpoint_destroy(ep1a);
    free(cha);
    free(epa);
    free(ch1a);
    free(ep1a);
  }

  TEST_ASSERT(e == RC_OK);
}

void message_test() {
  MAM2_TRITS_DEF0(key_a, MAM2_PRNG_KEY_SIZE);
  key_a = MAM2_TRITS_INIT(key_a, MAM2_PRNG_KEY_SIZE);
  trits_from_str(key_a, TEST_PRNG_A_KEY);

  MAM2_TRITS_DEF0(key_b, MAM2_PRNG_KEY_SIZE);
  key_b = MAM2_TRITS_INIT(key_b, MAM2_PRNG_KEY_SIZE);
  trits_from_str(key_b, TEST_PRNG_B_KEY);

  mam_prng_t prng_sender;
  mam_prng_t prng_receiver;

  mam_prng_init(&prng_sender, key_a);
  mam_prng_init(&prng_receiver, key_b);

  message_test_generic(&prng_sender, &prng_receiver);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(message_test);
  return UNITY_END();
}
