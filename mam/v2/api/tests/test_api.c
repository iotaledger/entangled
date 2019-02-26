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

#include "common/trinary/tryte_ascii.h"
#include "mam/v2/api/api.h"
#include "mam/v2/mam/message.h"
#include "mam/v2/ntru/mam_ntru_sk_t_set.h"
#include "mam/v2/test_utils/test_utils.h"

static tryte_t API_SEED[] =
    "APISEEDAPISEEDAPISEEDAPISEEDAPISEEDAPISEEDAPISEEDAPISEEDAPISEEDAPISEEDAPIS"
    "EEDAPI9";

#define TEST_CHANNEL_NAME "CHANAME"
#define TEST_CHANNEL_1_NAME "CHANAME9"
#define TEST_END_POINT_NAME "EPANAME"
#define TEST_END_POINT_1_NAME "EPANAME9"
#define TEST_PRE_SHARED_KEY_A_STR "PSKIDAPSKIDAPSKIDAPSKIDAPSK"
#define TEST_PRE_SHARED_KEY_A_NONCE_STR "PSKANONCE"
#define TEST_PRE_SHARED_KEY_B_STR "PSKIDBPSKIDBPSKIDBPSKIDBPSK"
#define TEST_PRE_SHARED_KEY_B_NONCE_STR "PSKBNONCE"
#define TEST_NTRU_NONCE "NTRUBNONCE"
#define TEST_MSS_DEPTH 1

static void test_api_write_header(
    mam_api_t *const api, mam_psk_t const *const pska,
    mam_psk_t const *const pskb, mam_ntru_pk_t const *const ntru_pk,
    mam_msg_pubkey_t pubkey, mam_msg_keyload_t keyload,
    mam_channel_t *const cha, mam_endpoint_t *const epa,
    mam_channel_t *const ch1a, mam_endpoint_t *const ep1a,
    bundle_transactions_t *const bundle, trit_t *const msg_id) {
  mam_channel_t *ch = cha;
  mam_endpoint_t *ep = NULL;
  mam_channel_t *ch1 = NULL;
  mam_endpoint_t *ep1 = NULL;
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

  TEST_ASSERT(mam_api_bundle_write_header(api, ch, ep, ch1, ep1, psks, ntru_pks,
                                          0, bundle, msg_id) == RC_OK);

  mam_psk_t_set_free(&psks);
  mam_ntru_pk_t_set_free(&ntru_pks);
}

static trits_t test_api_write_packet(
    mam_api_t *const api, bundle_transactions_t *const bundle,
    trit_t *const msg_id, mam_msg_pubkey_t pubkey, mam_msg_checksum_t checksum,
    mam_channel_t *const cha, mam_endpoint_t *const epa,
    mam_channel_t *const ch1a, mam_endpoint_t *const ep1a,
    char const *payload1) {
  // trits_t packet = trits_null();
  // trits_t payload = trits_null();
  //
  // if (mam_msg_checksum_mssig == checksum) {
  //   if (mam_msg_pubkey_chid == pubkey) {
  //     send_ctx->mss = &cha->mss;
  //   } else if (mam_msg_pubkey_epid == pubkey) {
  //     send_ctx->mss = &epa->mss;
  //   } else if (mam_msg_pubkey_chid1 == pubkey) {
  //     send_ctx->mss = &ch1a->mss;
  //   } else if (mam_msg_pubkey_epid1 == pubkey) {
  //     send_ctx->mss = &ep1a->mss;
  //   }
  // }
  //

  tryte_t *payload_trytes =
      (tryte_t *)malloc(2 * strlen(payload1) * sizeof(tryte_t));

  ascii_to_trytes(payload1, payload_trytes);
  mam_api_bundle_write_packet(api, cha, msg_id, payload_trytes,
                              strlen(payload1) * 2, checksum, bundle);
  free(payload_trytes);
}

static void message_test_generic_receive_msg(
    mam_api_t *const api, bundle_transactions_t *const bundle,
    mam_psk_t const *const pre_shared_key, mam_ntru_sk_t const *const ntru,
    mam_channel_t *const cha, char **payload2) {
  // mam_psk_t_set_t psks = NULL;
  // mam_ntru_sk_t_set_t ntru_sks = NULL;
  // MAM2_TRITS_DEF0(msg_id, MAM2_MSG_ID_SIZE);
  // msg_id = MAM2_TRITS_INIT(msg_id, MAM2_MSG_ID_SIZE);
  //
  // TEST_ASSERT(mam_psk_t_set_add(&psks, pre_shared_key) == RC_OK);
  // TEST_ASSERT(mam_ntru_sk_t_set_add(&ntru_sks, ntru) == RC_OK);
  //
  // trits_copy(mam_channel_id(cha),
  //            trits_from_rep(MAM2_CHANNEL_ID_SIZE, cfg->pk));
  //
  // e = mam_msg_recv(cfg_msg_recv, msg, psks, ntru_sks, msg_id);
  //
  // TEST_ASSERT(RC_OK == e);
  // TEST_ASSERT(trits_is_empty(*msg));
  // MAM2_ASSERT(trits_cmp_eq_str(msg_id, "SENDERMSGIDAAAAASENDERMSGID"));
  //
  // mam_ntru_sk_t_set_free(&ntru_sks);
  // mam_psk_t_set_free(&psks);

  tryte_t *payload_trytes = NULL;
  size_t payload_size = 0;

  TEST_ASSERT(mam_api_bundle_read_msg(api, bundle, &payload_trytes,
                                      &payload_size) == RC_OK);
  *payload2 = calloc(payload_size * 2 + 1, sizeof(char));

  trytes_to_ascii(payload_trytes, payload_size, *payload2);

  fprintf(stderr, "[%s]\n", *payload2);
}

static void message_test_generic_receive_packet(trits_t const *const packet,
                                                trits_t *const payload) {
  // retcode_t e = RC_MAM2_INTERNAL_ERROR;
  // {
  //   e = mam_msg_recv_packet(ctx, packet, payload);
  //   TEST_ASSERT(RC_OK == e);
  //   TEST_ASSERT(trits_is_empty(*packet));
  //   TEST_ASSERT(trits_is_empty(*payload));
  //   *payload = trits_pickup_all(*payload);
  // }
}

static void message_test_create_channels(mam_api_t *api,
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
    e = mam_channel_create(&api->prng, d, cha_name, *cha);
    TEST_ASSERT(RC_OK == e);

    /* create endpoints */
    {
      trits_t epa_name = trits_alloc(3 * strlen(TEST_END_POINT_NAME));
      trits_from_str(epa_name, TEST_END_POINT_NAME);

      *epa = malloc(sizeof(mam_endpoint_t));
      TEST_ASSERT(0 != *epa);
      memset(*epa, 0, sizeof(mam_endpoint_t));
      e = mam_endpoint_create(&api->prng, d, (*cha)->name, epa_name, *epa);
      TEST_ASSERT(RC_OK == e);
      trits_free(epa_name);
    }
    {
      trits_t ep1a_name = trits_alloc(3 * strlen(TEST_END_POINT_1_NAME));
      trits_from_str(ep1a_name, TEST_END_POINT_1_NAME);

      *ep1 = malloc(sizeof(mam_endpoint_t));
      TEST_ASSERT(0 != *ep1);
      memset(*ep1, 0, sizeof(mam_endpoint_t));
      e = mam_endpoint_create(&api->prng, d, (*cha)->name, ep1a_name, *ep1);
      TEST_ASSERT(RC_OK == e);
      trits_free(ep1a_name);
    }
    {
      trits_t ch1a_name = trits_alloc(3 * strlen(TEST_CHANNEL_1_NAME));
      trits_from_str(ch1a_name, TEST_CHANNEL_1_NAME);

      *ch1 = malloc(sizeof(mam_channel_t));
      TEST_ASSERT(0 != *ch1);
      memset(*ch1, 0, sizeof(mam_channel_t));
      e = mam_channel_create(&api->prng, d, ch1a_name, *ch1);
      TEST_ASSERT(RC_OK == e);
      trits_free(ch1a_name);
    }
    trits_free(cha_name);
  }
}

static void api_test_generic(mam_api_t *const api) {
  retcode_t e = RC_OK;
  bundle_transactions_t *bundle = NULL;
  trit_t msg_id[MAM2_MSG_ID_SIZE];

  mam_channel_t *cha = NULL, *ch1a = NULL;
  mam_endpoint_t *epa = NULL, *ep1a = NULL;

  mam_msg_pubkey_t pubkey;     /* chid=0, epid=1, chid1=2, epid1=3 */
  mam_msg_keyload_t keyload;   /* public=0, psk=1, ntru=2 */
  mam_msg_checksum_t checksum; /* none=0, mac=1, mssig=2 */

  message_test_create_channels(api, &cha, &ch1a, &epa, &ep1a);

  char const *payload1 = "Test";
  char *payload2 = NULL;

  mam_ntru_sk_t ntru[1];

  /* gen recipient'spongos ntru keys, public key is shared with sender */
  {
    retcode_t e;
    MAM2_TRITS_DEF0(ntru_nonce, 30);
    ntru_nonce = MAM2_TRITS_INIT(ntru_nonce, 30);
    trits_from_str(ntru_nonce, TEST_NTRU_NONCE);

    e = ntru_init(ntru);
    ntru_gen(ntru, &api->prng, ntru_nonce);
    TEST_ASSERT(RC_OK == e);
    mam_api_add_ntru_sk(api, ntru);
  }

  mam_psk_t pska[1], pskb[1];

  /* gen psk */
  {
    trits_from_str(mam_psk_id(pska), TEST_PRE_SHARED_KEY_A_STR);
    prng_gen_str(&api->prng, MAM2_PRNG_DST_SEC_KEY,
                 TEST_PRE_SHARED_KEY_A_NONCE_STR, mam_psk_trits(pska));
    // mam_api_add_psk(api, pska);

    trits_from_str(mam_psk_id(pskb), TEST_PRE_SHARED_KEY_B_STR);
    prng_gen_str(&api->prng, MAM2_PRNG_DST_SEC_KEY,
                 TEST_PRE_SHARED_KEY_B_NONCE_STR, mam_psk_trits(pskb));
    mam_api_add_psk(api, pskb);
  }

  /* chid=0, epid=1, chid1=2, epid1=3*/
  for (pubkey = 0; pubkey < 4; ++pubkey) {
    /* public=0, psk=1, ntru=2 */
    for (keyload = 0; keyload < 3; ++keyload) {
      /* none=0, mac=1, mssig=2 */
      for (checksum = 0; checksum < 1 /*3*/; ++checksum) {
        fprintf(stderr, "pubkey %d, keyload %d, checksum %d\n", pubkey, keyload,
                checksum);
        bundle_transactions_new(&bundle);

        /* send msg and packet */
        test_api_write_header(api, pska, pskb, &ntru->public_key, pubkey,
                              keyload, cha, epa, ch1a, ep1a, bundle, msg_id);
        test_api_write_packet(api, bundle, msg_id, pubkey, checksum, cha, epa,
                              ch1a, ep1a, payload1);

        /* recv msg and packet */
        message_test_generic_receive_msg(api, bundle, pskb, ntru, cha,
                                         &payload2);
        // message_test_generic_receive_packet(&packet, &payload);
        TEST_ASSERT_EQUAL_STRING(payload1, payload2);

        /* cleanup */
        bundle_transactions_free(&bundle);
        free(payload2);
        payload2 = NULL;
      }
    }
  }

  ntru_destroy(ntru);

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

void test_api() {
  mam_api_t api;

  TEST_ASSERT(mam_api_init(&api, API_SEED) == RC_OK);
  api_test_generic(&api);
  TEST_ASSERT(mam_api_destroy(&api) == RC_OK);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_api);

  return UNITY_END();
}
