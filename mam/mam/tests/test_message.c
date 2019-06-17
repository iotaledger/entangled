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

#include "mam/mam/mam_channel_t_set.h"
#include "mam/mam/message.h"
#include "mam/ntru/mam_ntru_sk_t_set.h"

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

#define TEST_PLAINTEXT1 "WHATANONSENSEMESSAGE"

#define TEST_MSS_DEPTH 1

static mam_prng_t prng_sender;
static mam_prng_t prng_receiver;

static trits_t message_test_generic_write_msg(mam_prng_t *prng, mam_psk_t const *const pska,
                                              mam_psk_t const *const pskb, mam_ntru_pk_t const *const ntru_pk,
                                              mam_msg_pubkey_t pubkey, mam_msg_keyload_t keyload,
                                              mam_channel_t *const cha, mam_endpoint_t *const epa,
                                              mam_channel_t *const ch1a, mam_endpoint_t *const ep1a,
                                              mam_msg_write_context_t *const write_ctx, trits_t msg_id) {
  trits_t msg = trits_null();
  mam_channel_t *ch = cha;
  mam_endpoint_t *ep = NULL;
  mam_channel_t *ch1 = NULL;
  mam_endpoint_t *ep1 = NULL;
  trint9_t msg_type_id = 0;
  mam_psk_t_set_t psks = NULL;
  mam_ntru_pk_t_set_t ntru_pks = NULL;

  if (MAM_MSG_PUBKEY_EPID == pubkey) {
    ep = epa;
  } else if (MAM_MSG_PUBKEY_CHID1 == pubkey) {
    ch1 = ch1a;
  } else if (MAM_MSG_PUBKEY_EPID1 == pubkey) {
    ep1 = ep1a;
  }

  if (MAM_MSG_KEYLOAD_PSK == keyload) {
    mam_psk_t_set_add(&psks, pska);
    mam_psk_t_set_add(&psks, pskb);
  } else if (MAM_MSG_KEYLOAD_NTRU == keyload) {
    mam_ntru_pk_t_set_add(&ntru_pks, ntru_pk);
  }

  size_t sz;
  sz = mam_msg_header_size(ch, ep, ch1, ep1, psks, ntru_pks);
  msg = trits_alloc(sz);
  TEST_ASSERT(!trits_is_null(msg));

  TEST_ASSERT_EQUAL_INT(
      RC_OK, mam_msg_write_header(write_ctx, prng, ch, ep, ch1, ep1, msg_id, msg_type_id, psks, ntru_pks, &msg));
  TEST_ASSERT(trits_is_empty(msg));
  msg = trits_pickup(msg, sz);
  mam_psk_t_set_free(&psks);
  mam_ntru_pk_t_set_free(&ntru_pks);

  return msg;
}

static trits_t message_test_generic_write_first_packet(mam_msg_pubkey_t pubkey, mam_msg_checksum_t checksum,
                                                       mam_channel_t *const cha, mam_endpoint_t *const epa,
                                                       mam_channel_t *const ch1a, mam_endpoint_t *const ep1a,
                                                       mam_msg_write_context_t *const write_ctx,
                                                       char const *payload_str) {
  trits_t packet = trits_null();
  trits_t payload = trits_null();

  if (MAM_MSG_CHECKSUM_SIG == checksum) {
    if (MAM_MSG_PUBKEY_CHID == pubkey) {
      write_ctx->mss = &cha->mss;
    } else if (MAM_MSG_PUBKEY_EPID == pubkey) {
      write_ctx->mss = &epa->mss;
    } else if (MAM_MSG_PUBKEY_CHID1 == pubkey) {
      write_ctx->mss = &ch1a->mss;
    } else if (MAM_MSG_PUBKEY_EPID1 == pubkey) {
      write_ctx->mss = &ep1a->mss;
    }
  }

  size_t sz;
  payload = trits_alloc(3 * strlen(payload_str));
  TEST_ASSERT(!trits_is_null(payload));
  trits_from_str(payload, payload_str);

  sz = mam_msg_packet_size(checksum, write_ctx->mss, trits_size(payload));
  packet = trits_alloc(sz);
  TEST_ASSERT(!trits_is_null(packet));

  TEST_ASSERT_EQUAL_INT(RC_OK, mam_msg_write_packet(write_ctx, checksum, payload, &packet));
  TEST_ASSERT(trits_is_empty(packet));
  packet = trits_pickup(packet, sz);
  trits_set_zero(payload);
  trits_free(payload);

  return packet;
}

static void message_test_generic_read_msg(mam_psk_t const *const pre_shared_key, mam_ntru_sk_t const *const ntru,
                                          mam_channel_t *const cha, mam_endpoint_t *const ep, trits_t *const msg,
                                          mam_msg_read_context_t *const cfg_msg_read, trits_t msg_id) {
  retcode_t e = RC_MAM_INTERNAL_ERROR;

  /* init read msg context */
  mam_msg_read_context_t *cfg = cfg_msg_read;
  mam_psk_t_set_t psks = NULL;
  mam_ntru_sk_t_set_t ntru_sks = NULL;
  mam_pk_t_set_t trusted_channel_ids = NULL;
  mam_pk_t_set_t trusted_endpoint_ids = NULL;
  mam_pk_t pk;

  TEST_ASSERT(mam_psk_t_set_add(&psks, pre_shared_key) == RC_OK);
  TEST_ASSERT(mam_ntru_sk_t_set_add(&ntru_sks, ntru) == RC_OK);
  memcpy(pk.key, trits_begin(mam_channel_id(cha)), MAM_CHANNEL_ID_TRIT_SIZE);
  TEST_ASSERT(mam_pk_t_set_add(&trusted_channel_ids, &pk) == RC_OK);
  if (ep) {
    memcpy(pk.key, trits_begin(mam_endpoint_id(ep)), MAM_ENDPOINT_ID_TRIT_SIZE);
    TEST_ASSERT(mam_pk_t_set_add(&trusted_endpoint_ids, &pk) == RC_OK);
  }

  trits_copy(mam_channel_id(cha), trits_from_rep(MAM_CHANNEL_ID_TRIT_SIZE, cfg->pk.key));

  e = mam_msg_read_header(cfg_msg_read, msg, psks, ntru_sks, msg_id, &trusted_channel_ids, &trusted_endpoint_ids);

  TEST_ASSERT(RC_OK == e);
  TEST_ASSERT(trits_is_empty(*msg));
  mam_pk_t_set_free(&trusted_channel_ids);
  mam_pk_t_set_free(&trusted_endpoint_ids);

  mam_ntru_sk_t_set_free(&ntru_sks);
  mam_psk_t_set_free(&psks);
}

static void message_test_generic_read_packet(mam_msg_read_context_t *const ctx, trits_t *const packet,
                                             trits_t *const payload) {
  retcode_t e = RC_MAM_INTERNAL_ERROR;
  ctx->ord = 0;

  e = mam_msg_read_packet(ctx, packet, payload);
  TEST_ASSERT(RC_OK == e);
  TEST_ASSERT(trits_is_empty(*packet));
  TEST_ASSERT(trits_is_empty(*payload));
  *payload = trits_pickup_all(*payload);
}

static void message_test_create_channels(mam_prng_t *prng, mam_channel_t **const cha, mam_channel_t **const ch1,
                                         mam_endpoint_t **const epa, mam_endpoint_t **ep1) {
  retcode_t e = RC_MAM_INTERNAL_ERROR;
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
      e = mam_endpoint_create(prng, d, mam_channel_name_size(*cha), mam_channel_name(*cha), epa_name, *epa);
      TEST_ASSERT(RC_OK == e);
      trits_free(epa_name);
    }
    {
      trits_t ep1a_name = trits_alloc(3 * strlen(TEST_END_POINT_1_NAME));
      trits_from_str(ep1a_name, TEST_END_POINT_1_NAME);

      *ep1 = malloc(sizeof(mam_endpoint_t));
      TEST_ASSERT(0 != *ep1);
      memset(*ep1, 0, sizeof(mam_endpoint_t));
      e = mam_endpoint_create(prng, d, mam_channel_name_size(*cha), mam_channel_name(*cha), ep1a_name, *ep1);
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

static void message_test_generic(mam_prng_t *prng_sender, mam_prng_t *prng_receiver) {
  retcode_t e = RC_OK;

  trits_t msg = trits_null(), packet = trits_null(), payload = trits_null();

  mam_msg_write_context_t write_ctx;
  mam_spongos_init(&write_ctx.spongos);
  write_ctx.ord = 0;
  write_ctx.mss = NULL;

  mam_msg_read_context_t cfg_msg_read[1];

  mam_msg_pubkey_t pubkey;     /* chid=0, epid=1, chid1=2, epid1=3 */
  mam_msg_keyload_t keyload;   /* psk=1, ntru=2 */
  mam_msg_checksum_t checksum; /* none=0, mac=1, mssig=2 */

  char const *payload_str = "PAYLOAD9999";
  payload = trits_alloc(3 * strlen(payload_str));

  mam_ntru_sk_t ntru[1];

  /* gen recipient'spongos ntru keys, public key is shared with sender */
  {
    retcode_t e;
    MAM_TRITS_DEF(ntru_nonce, 30);
    ntru_nonce = MAM_TRITS_INIT(ntru_nonce, 30);
    trits_from_str(ntru_nonce, TEST_NTRU_NONCE);

    e = ntru_sk_reset(ntru);
    ntru_sk_gen(ntru, prng_receiver, ntru_nonce);
    TEST_ASSERT(RC_OK == e);
  }

  mam_psk_t pska, pskb;

  /* gen psk */
  {
    mam_psk_gen(&pska, prng_sender, (tryte_t *)TEST_PRE_SHARED_KEY_A_STR, (tryte_t *)TEST_PRE_SHARED_KEY_A_NONCE_STR,
                strlen(TEST_PRE_SHARED_KEY_A_NONCE_STR));

    mam_psk_gen(&pskb, prng_receiver, (tryte_t *)TEST_PRE_SHARED_KEY_B_STR, (tryte_t *)TEST_PRE_SHARED_KEY_B_NONCE_STR,
                strlen(TEST_PRE_SHARED_KEY_B_NONCE_STR));
  }

  MAM_TRITS_DEF(msg_id, MAM_MSG_ID_SIZE);
  msg_id = MAM_TRITS_INIT(msg_id, MAM_MSG_ID_SIZE);
  trits_from_str(msg_id, "SENDERMSGIDAAAAASENDERMSGID");

  mam_endpoint_t *curr_ep;

  /* chid=0, epid=1, chid1=2, epid1=3*/
  for (pubkey = 0; (int)pubkey < 4; ++pubkey) {
    /* public=0, psk=1, ntru=2 */
    for (keyload = 0; (int)keyload < 3; ++keyload) {
      for (checksum = 0; (int)checksum < 3; ++checksum)
      /* none=0, mac=1, mssig=2 */
      {
        mam_channel_t *cha = NULL, *ch1a = NULL;
        mam_endpoint_t *epa = NULL, *ep1a = NULL;
        message_test_create_channels(prng_sender, &cha, &ch1a, &epa, &ep1a);
        /* write msg and packet */
        {
          msg = message_test_generic_write_msg(prng_sender, &pska, &pskb, &ntru->public_key, pubkey, keyload, cha, epa,
                                               ch1a, ep1a, &write_ctx, msg_id);

          packet =
              message_test_generic_write_first_packet(pubkey, checksum, cha, epa, ch1a, ep1a, &write_ctx, payload_str);
        }

        if (pubkey == MAM_MSG_PUBKEY_EPID) {
          curr_ep = epa;
        } else {
          curr_ep = NULL;
        }

        /* read msg and packet */
        {
          message_test_generic_read_msg(&pskb, ntru, cha, curr_ep, &msg, cfg_msg_read, msg_id);

          message_test_generic_read_packet(cfg_msg_read, &packet, &payload);
          TEST_ASSERT(trits_cmp_eq_str(payload, payload_str));
        }

        /* cleanup */
        {
          trits_free(msg);
          trits_free(packet);
        }

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
      }
    }
  }

  ntru_sk_reset(ntru);

  trits_free(payload);

  TEST_ASSERT(e == RC_OK);
}

void message_test() {
  MAM_TRITS_DEF(key_a, MAM_PRNG_SECRET_KEY_SIZE);
  key_a = MAM_TRITS_INIT(key_a, MAM_PRNG_SECRET_KEY_SIZE);
  trits_from_str(key_a, TEST_PRNG_A_KEY);

  MAM_TRITS_DEF(key_b, MAM_PRNG_SECRET_KEY_SIZE);
  key_b = MAM_TRITS_INIT(key_b, MAM_PRNG_SECRET_KEY_SIZE);
  trits_from_str(key_b, TEST_PRNG_B_KEY);

  mam_prng_init(&prng_sender, key_a);
  mam_prng_init(&prng_receiver, key_b);

  message_test_generic(&prng_sender, &prng_receiver);
}

void serialize_write_ctx_test() {
  mam_msg_write_context_t write_ctx;
  mam_msg_write_context_t deserialized_ctx;
  mam_channel_t ch;
  mam_channel_t_set_t chs = NULL;

  mam_spongos_init(&write_ctx.spongos);
  write_ctx.ord = 0;

  MAM_TRITS_DEF(ch_name, 3 * strlen(TEST_CHANNEL_NAME));
  ch_name = MAM_TRITS_INIT(ch_name, 3 * strlen(TEST_CHANNEL_NAME));
  trits_from_str(ch_name, TEST_CHANNEL_NAME);

  TEST_ASSERT(mam_channel_create(&prng_sender, 2, ch_name, &ch) == RC_OK);
  TEST_ASSERT(mam_channel_t_set_add(&chs, &ch) == RC_OK);

  memcpy(write_ctx.chid, trits_begin(mam_channel_id(&ch)), MAM_CHANNEL_ID_TRIT_SIZE);
  write_ctx.mss = &chs->value.mss;

  MAM_TRITS_DEF(rand_msg, strlen(TEST_PLAINTEXT1));
  rand_msg = MAM_TRITS_INIT(rand_msg, strlen(TEST_PLAINTEXT1));
  trits_from_str(rand_msg, TEST_PLAINTEXT1);
  mam_spongos_absorb(&write_ctx.spongos, rand_msg);
  MAM_TRITS_DEF(ctx_buffer, mam_msg_write_ctx_serialized_size(&write_ctx));
  ctx_buffer = MAM_TRITS_INIT(ctx_buffer, mam_msg_write_ctx_serialized_size(&write_ctx));
  mam_msg_write_ctx_serialize(&write_ctx, &ctx_buffer);
  ctx_buffer = trits_pickup_all(ctx_buffer);
  TEST_ASSERT(mam_msg_write_ctx_deserialize(&ctx_buffer, &deserialized_ctx, chs) == RC_OK);
  TEST_ASSERT_EQUAL_MEMORY(write_ctx.chid, deserialized_ctx.chid, MAM_CHANNEL_ID_TRIT_SIZE);
  TEST_ASSERT_EQUAL_INT(write_ctx.spongos.pos, deserialized_ctx.spongos.pos);
  TEST_ASSERT_EQUAL_MEMORY(&write_ctx.spongos.sponge, &deserialized_ctx.spongos.sponge, MAM_SPONGE_WIDTH);
  TEST_ASSERT_EQUAL_INT(write_ctx.mss, deserialized_ctx.mss);
  TEST_ASSERT_EQUAL_INT(write_ctx.ord, deserialized_ctx.ord);

  mam_channels_destroy(&chs);
}

void serialize_read_ctx_test() {
  mam_msg_read_context_t read_ctx;
  mam_msg_read_context_t deserialized_ctx;
  mam_spongos_init(&read_ctx.spongos);
  read_ctx.ord = 0;

  //"Random" root
  for (size_t i = 0; i < MAM_CHANNEL_ID_TRIT_SIZE; ++i) {
    read_ctx.pk.key[i] = -1 + rand() % 3;
  }

  MAM_TRITS_DEF(rand_msg, strlen(TEST_PLAINTEXT1));
  rand_msg = MAM_TRITS_INIT(rand_msg, strlen(TEST_PLAINTEXT1));
  trits_from_str(rand_msg, TEST_PLAINTEXT1);
  mam_spongos_absorb(&read_ctx.spongos, rand_msg);
  MAM_TRITS_DEF(ctx_buffer, mam_msg_read_ctx_serialized_size(&read_ctx));
  ctx_buffer = MAM_TRITS_INIT(ctx_buffer, mam_msg_read_ctx_serialized_size(&read_ctx));
  mam_msg_read_ctx_serialize(&read_ctx, &ctx_buffer);
  ctx_buffer = trits_pickup_all(ctx_buffer);
  mam_msg_read_ctx_deserialize(&ctx_buffer, &deserialized_ctx);
  TEST_ASSERT_EQUAL_INT(read_ctx.spongos.pos, deserialized_ctx.spongos.pos);
  TEST_ASSERT_EQUAL_MEMORY(&read_ctx.spongos.sponge, &deserialized_ctx.spongos.sponge, MAM_SPONGE_WIDTH);

  TEST_ASSERT_EQUAL_MEMORY(&read_ctx.pk, &deserialized_ctx.pk, MAM_CHANNEL_ID_TRIT_SIZE);
  TEST_ASSERT_EQUAL_INT(read_ctx.ord, deserialized_ctx.ord);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(message_test);
  RUN_TEST(serialize_write_ctx_test);
  RUN_TEST(serialize_read_ctx_test);

  return UNITY_END();
}
