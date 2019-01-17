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

mam_channel_t *test_create_channel(char const *const cha_name_str,
                                   prng_t const *const prng,
                                   mss_mt_height_t d) {
  mam_channel_t *cha = NULL;
  mam_ialloc_t ma[1];
  ma->create_sponge = test_create_sponge;
  ma->destroy_sponge = test_delete_sponge;

  trits_t cha_name = trits_alloc(3 * strlen(cha_name_str));
  trits_from_str(cha_name, cha_name_str);

  cha = malloc(sizeof(mam_channel_t));
  TEST_ASSERT(0 != cha);
  memset(cha, 0, sizeof(mam_channel_t));
  TEST_ASSERT(mam_channel_create(ma, prng, d, cha_name, cha) == RC_OK);

  trits_free(cha_name);

  return cha;
}

void test_gen_psk(mam_psk_node *const psk, prng_t const *const prng,
                  char const *const id, char const *const nonce) {
  psk->prev = psk->next = 0;
  trits_from_str(mam_psk_id(&psk->info), "PSKIDBPSKIDBPSKIDBPSKIDBPSK");
  prng_gen_str(prng, MAM2_PRNG_DST_SECKEY, "PSKBNONCE",
               mam_psk_trits(&psk->info));
}

void test_init_send_msg_cfg(mam_send_msg_context_t *const cfg,
                            mam_psk_node *const psk, prng_t const *const prng,
                            mam_channel_t const *const cha,
                            sponge_t *const spongos_sponge,
                            sponge_t *const fork_sponge) {
  cfg->ma = NULL;
  cfg->spongos->sponge = spongos_sponge;
  cfg->fork->sponge = fork_sponge;
  cfg->prng = prng;
  cfg->rng = prng;

  cfg->ch = cha;
  cfg->ch1 = 0;
  cfg->ep = 0;
  cfg->ep1 = 0;

  cfg->key_plain = 0;
  cfg->psks.begin = 0;
  cfg->psks.end = 0;
  cfg->ntru_pks.begin = 0;
  cfg->ntru_pks.end = 0;

  psk->prev = psk->next = 0;
  mam_list_insert_end(cfg->psks, psk);
}

static trits_t mam_test_generic_send_msg(
    void *sponge_alloc_ctx, sponge_t *(create_sponge)(void *ctx),
    void (*destroy_sponge)(void *ctx, sponge_t *), prng_t *pa, prng_t *pb,
    mam_msg_pubkey_t pubkey, mam_msg_keyload_t keyload,
    mam_msg_checksum_t checksum, mam_channel_t *const cha,
    mam_endpoint_t *const epa, mam_channel_t *const ch1a,
    mam_endpoint_t *const ep1a, mam_send_msg_context_t *const cfg_msga) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;

  trits_t msg = trits_null();
  mss_mt_height_t d = 1;
  mam_psk_node pska[1], pskb[1];
  ntru_t nb[1];
  mam_ntru_pk_node nbpk[1];
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

  {
    MAM2_TRITS_DEF0(k, MAM2_PRNG_KEY_SIZE);
    k = MAM2_TRITS_INIT(k, MAM2_PRNG_KEY_SIZE);

    trits_from_str(k,
                   "SENDERPRNGKEYASENDERPRNGKEY"
                   "SENDERPRNGKEYASENDERPRNGKEY"
                   "SENDERPRNGKEYASENDERPRNGKEY");
    prng_init(pa, pa->sponge, k);

    trits_from_str(k,
                   "RECIPIPRNGKEYBRECIPIPRNGKEY"
                   "RECIPIPRNGKEYBRECIPIPRNGKEY"
                   "RECIPIPRNGKEYBRECIPIPRNGKEY");
    prng_init(pb, pb->sponge, k);
  }

  /* gen psk */
  {
    pska->prev = pska->next = 0;
    trits_from_str(mam_psk_id(&pska->info), "PSKIDAPSKIDAPSKIDAPSKIDAPSK");
    prng_gen_str(pa, MAM2_PRNG_DST_SECKEY, "PSKANONCE",
                 mam_psk_trits(&pska->info));

    pskb->prev = pskb->next = 0;
    trits_from_str(mam_psk_id(&pskb->info), "PSKIDBPSKIDBPSKIDBPSKIDBPSK");
    prng_gen_str(pb, MAM2_PRNG_DST_SECKEY, "PSKBNONCE",
                 mam_psk_trits(&pskb->info));
  }
  /* gen recipient'spongos ntru keys */
  {
    MAM2_TRITS_DEF0(N, 30);
    N = MAM2_TRITS_INIT(N, 30);
    trits_from_str(N, "NTRUBNONCE");

    e = ntru_create(nb);
    ntru_gen(nb, pb, N, mam_ntru_pk_trits(&nbpk->info));
    TEST_ASSERT(RC_OK == e);
  }
  {
    mam_send_msg_context_t *cfg = cfg_msga;

    cfg->ma = 0;
    cfg->spongos->sponge = sponge_send;
    cfg->fork->sponge = fork_sponge_send;
    cfg->prng = pa;
    cfg->rng = pa;
    cfg->ns->sponge = ntru_sponge_send;

    cfg->ch = cha;
    cfg->ch1 = 0;
    cfg->ep = 0;
    cfg->ep1 = 0;
    if (mam_msg_pubkey_epid == pubkey)
      cfg->ep = epa;
    else if (mam_msg_pubkey_chid1 == pubkey)
      cfg->ch1 = ch1a;
    else if (mam_msg_pubkey_epid1 == pubkey)
      cfg->ep1 = ep1a;
    else
      ;

    cfg->key_plain = 0;
    cfg->psks.begin = 0;
    cfg->psks.end = 0;
    cfg->ntru_pks.begin = 0;
    cfg->ntru_pks.end = 0;
    if (mam_msg_keyload_plain == keyload)
      cfg->key_plain = 1;
    else if (mam_msg_keyload_psk == keyload) {
      pska->prev = pska->next = 0;
      pskb->prev = pskb->next = 0;
      mam_list_insert_end(cfg->psks, pska);
      mam_list_insert_end(cfg->psks, pskb);
    } else if (mam_msg_keyload_ntru == keyload) {
      nbpk->prev = nbpk->next = 0;
      mam_list_insert_end(cfg->ntru_pks, nbpk);
    } else
      ;

    trits_from_str(mam_send_msg_cfg_nonce(cfg), "SENDERNONCEAAAAASENDERNONCE");
  }

  size_t sz;
  sz = mam_send_msg_size(cfg_msga);
  msg = trits_alloc(sz);
  TEST_ASSERT(!trits_is_null(msg));

  mam_send_msg(cfg_msga, &msg);
  TEST_ASSERT(trits_is_empty(msg));
  msg = trits_pickup(msg, sz);

  ntru_destroy(nb);

  return msg;
}

static trits_t mam_test_generic_send_packet(
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
      if (mam_msg_pubkey_chid == pubkey)
        cfg->mss = cha->m;
      else if (mam_msg_pubkey_epid == pubkey)
        cfg->mss = epa->m;
      else if (mam_msg_pubkey_chid1 == pubkey)
        cfg->mss = ch1a->m;
      else if (mam_msg_pubkey_epid1 == pubkey)
        cfg->mss = ep1a->m;
      else
        ;
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

  return packet;
}

static void mam_test_generic_receive_msg(
    void *sponge_alloc_ctx, sponge_t *(create_sponge)(void *ctx),
    void (*destroy_sponge)(void *ctx, sponge_t *), prng_t *pa, prng_t *pb,
    mam_channel_t *const cha, trits_t *const msg,
    mam_recv_msg_context_t *const cfg_msg_recv) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;

  mam_ialloc_t ma[1];
  mam_psk_node pskb[1];
  mam_ntru_pk_node nbpk[1];
  ntru_t nb[1];

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
    pskb->prev = pskb->next = 0;
    trits_from_str(mam_psk_id(&pskb->info), "PSKIDBPSKIDBPSKIDBPSKIDBPSK");
    prng_gen_str(pb, MAM2_PRNG_DST_SECKEY, "PSKBNONCE",
                 mam_psk_trits(&pskb->info));
  }
  /* gen recipient'spongos ntru keys */
  {
    MAM2_TRITS_DEF0(N, 30);
    N = MAM2_TRITS_INIT(N, 30);
    trits_from_str(N, "NTRUBNONCE");

    e = ntru_create(nb);
    ntru_gen(nb, pb, N, mam_ntru_pk_trits(&nbpk->info));
    TEST_ASSERT(RC_OK == e);
  }

  /* init recv msg context */
  {
    mam_recv_msg_context_t *cfg = cfg_msg_recv;

    cfg->ma = ma;
    cfg->pubkey = -1;
    cfg->spongos->sponge = sponge_recv;
    cfg->fork->sponge = fork_sponge_recv;
    cfg->spongos_mss->sponge = mss_sponge_recv;
    cfg->spongos_wots->sponge = wots_sponge_recv;
    cfg->spongos_ntru->sponge = ntru_sponge_recv;

    cfg->psk = &pskb->info;
    cfg->ntru = nb;

    trits_copy(mam_channel_id(cha), mam_recv_msg_cfg_chid(cfg));
  }

  e = mam_recv_msg(cfg_msg_recv, msg);
  TEST_ASSERT(RC_OK == e);
  TEST_ASSERT(trits_is_empty(*msg));
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

      cfg->ma = ma;
      cfg->spongos->sponge = cfg_msg_recv->spongos->sponge;
      cfg->spongos->pos = cfg_msg_recv->spongos->pos;
      cfg->ord = -1;
      cfg->pk = trits_null();
      if (mam_msg_pubkey_chid == cfg_msg_recv->pubkey)
        cfg->pk = mam_recv_msg_cfg_chid(cfg_msg_recv);
      else if (mam_msg_pubkey_epid == cfg_msg_recv->pubkey)
        cfg->pk = mam_recv_msg_cfg_epid(cfg_msg_recv);
      else if (mam_msg_pubkey_chid1 == cfg_msg_recv->pubkey)
        cfg->pk = mam_recv_msg_cfg_chid1(cfg_msg_recv);
      else if (mam_msg_pubkey_epid1 == cfg_msg_recv->pubkey)
        cfg->pk = mam_recv_msg_cfg_chid1(cfg_msg_recv);
      else
        ;
      cfg->ms->sponge = cfg_msg_recv->spongos_mss->sponge;
      cfg->ws->sponge = cfg_msg_recv->spongos_wots->sponge;
    }

    e = mam_recv_packet(cfg_packet_receive, packet, payload);
    TEST_ASSERT(RC_OK == e);
    TEST_ASSERT(trits_is_empty(*packet));
    TEST_ASSERT(trits_is_empty(*payload));
    *payload = trits_pickup_all(*payload);
  }
}

static void mam_test_generic(sponge_t *s, void *sponge_alloc_ctx,
                             sponge_t *(create_sponge)(void *ctx),
                             void (*destroy_sponge)(void *ctx, sponge_t *),
                             prng_t *pa, prng_t *pb) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  mam_ialloc_t ma[1];

  trits_t msg = trits_null(), packet = trits_null(), payload = trits_null();

  mss_mt_height_t d = 1;
  char const *cha_name_str = "CHANAME";
  char const *ch1a_name_str = "CHANAME9";
  char const *epa_name_str = "EPANAME";
  char const *ep1a_name_str = "EPANAME9";
  mam_channel_t *cha = 0, *ch1a = 0;
  mam_endpoint_t *epa = 0, *ep1a = 0;

  mam_psk_node pska[1];
  ntru_t nb[1];
  mam_ntru_pk_node nbpk[1];

  mam_send_msg_context_t cfg_msg_send[1];
  mam_recv_msg_context_t cfg_msg_recv[1];
  mam_recv_packet_context_t cfg_packet_receive[1];

  mam_msg_pubkey_t pubkey;     /* chid=0, epid=1, chid1=2, epid1=3 */
  mam_msg_keyload_t keyload;   /* plain=0, psk=1, ntru=2 */
  mam_msg_checksum_t checksum; /* none=0, mac=1, mssig=2 */

  /* init alloc */
  {
    ma->create_sponge = create_sponge;
    ma->destroy_sponge = destroy_sponge;
  }

  /* init rng */
  {
    MAM2_TRITS_DEF0(k, MAM2_PRNG_KEY_SIZE);
    k = MAM2_TRITS_INIT(k, MAM2_PRNG_KEY_SIZE);

    trits_from_str(k,
                   "SENDERPRNGKEYASENDERPRNGKEY"
                   "SENDERPRNGKEYASENDERPRNGKEY"
                   "SENDERPRNGKEYASENDERPRNGKEY");
    prng_init(pa, pa->sponge, k);

    trits_from_str(k,
                   "RECIPIPRNGKEYBRECIPIPRNGKEY"
                   "RECIPIPRNGKEYBRECIPIPRNGKEY"
                   "RECIPIPRNGKEYBRECIPIPRNGKEY");
    prng_init(pb, pb->sponge, k);
  }

  /* create channels */
  {
    trits_t cha_name = trits_alloc(3 * strlen(cha_name_str));
    trits_from_str(cha_name, cha_name_str);

    cha = malloc(sizeof(mam_channel_t));
    TEST_ASSERT(0 != cha);
    memset(cha, 0, sizeof(mam_channel_t));
    e = mam_channel_create(ma, pa, d, cha_name, cha);
    TEST_ASSERT(RC_OK == e);

    /* create endpoints */
    if (1) {
      trits_t epa_name = trits_alloc(3 * strlen(epa_name_str));
      trits_from_str(epa_name, epa_name_str);

      epa = malloc(sizeof(mam_endpoint_t));
      TEST_ASSERT(0 != epa);
      memset(epa, 0, sizeof(mam_endpoint_t));
      e = mam_endpoint_create(ma, pa, d, cha_name, epa_name, epa);
      TEST_ASSERT(RC_OK == e);
      trits_free(epa_name);
    }
    if (1) {
      trits_t ep1a_name = trits_alloc(3 * strlen(ep1a_name_str));
      trits_from_str(ep1a_name, ep1a_name_str);

      ep1a = malloc(sizeof(mam_endpoint_t));
      TEST_ASSERT(0 != ep1a);
      memset(ep1a, 0, sizeof(mam_endpoint_t));
      e = mam_endpoint_create(ma, pa, d, cha_name, ep1a_name, ep1a);
      TEST_ASSERT(RC_OK == e);
      trits_free(ep1a_name);
    }
    trits_free(cha_name);
  }
  if (1) {
    trits_t ch1a_name = trits_alloc(3 * strlen(ch1a_name_str));
    trits_from_str(ch1a_name, ch1a_name_str);

    ch1a = malloc(sizeof(mam_channel_t));
    TEST_ASSERT(0 != ch1a);
    memset(ch1a, 0, sizeof(mam_channel_t));
    e = mam_channel_create(ma, pa, d, ch1a_name, ch1a);
    TEST_ASSERT(RC_OK == e);
    trits_free(ch1a_name);
  }
  /* gen psk */
  {
    pska->prev = pska->next = 0;
    trits_from_str(mam_psk_id(&pska->info), "PSKIDAPSKIDAPSKIDAPSKIDAPSK");
    prng_gen_str(pa, MAM2_PRNG_DST_SECKEY, "PSKANONCE",
                 mam_psk_trits(&pska->info));
  }
  /* gen recipient'spongos ntru keys */
  {
    MAM2_TRITS_DEF0(N, 30);
    N = MAM2_TRITS_INIT(N, 30);
    trits_from_str(N, "NTRUBNONCE");

    e = ntru_create(nb);
    ntru_gen(nb, pb, N, mam_ntru_pk_trits(&nbpk->info));
    TEST_ASSERT(RC_OK == e);
  }

  for (pubkey = 0; pubkey < 4; ++pubkey) /* chid=0, epid=1, chid1=2, epid1=3
                                          */
    for (keyload = 0; keyload < 3; ++keyload) /* plain=0, psk=1, ntru=2 */
      for (checksum = 0; checksum < 3; ++checksum)
      /* none=0, mac=1, mssig=2 */
      {
        /* send/recv msg */
        {
          msg = mam_test_generic_send_msg(
              sponge_alloc_ctx, create_sponge, destroy_sponge, pa, pb, pubkey,
              keyload, checksum, cha, epa, ch1a, ep1a, cfg_msg_send);

          mam_test_generic_receive_msg(sponge_alloc_ctx, create_sponge,
                                       destroy_sponge, pa, pb, cha, &msg,
                                       cfg_msg_recv);
        }

        char const *payload_str = "PAYLOAD9999";
        payload = trits_alloc(3 * strlen(payload_str));
        packet =
            mam_test_generic_send_packet(pubkey, keyload, checksum, cha, epa,
                                         ch1a, ep1a, cfg_msg_send, payload_str);

        /* send/recv packet */
        {
          mam_test_generic_receive_packet(pubkey, keyload, checksum, cha, epa,
                                          ch1a, ep1a, cfg_msg_recv, &packet,
                                          &payload);
          TEST_ASSERT(trits_cmp_eq_str(payload, payload_str));
        }

        /* cleanup */
        {
          trits_free(msg);
          trits_free(packet);
          trits_free(payload);

          ma->destroy_sponge(cfg_msg_send->spongos->sponge);
          ma->destroy_sponge(cfg_msg_send->fork->sponge);
          ma->destroy_sponge(cfg_msg_send->ns->sponge);
        }
      }

  /* destroy channels/endpoints */
  {
    if (cha) mam_channel_destroy(ma, cha);
    if (ch1a) mam_channel_destroy(ma, ch1a);
    if (epa) mam_endpoint_destroy(ma, epa);
    if (ep1a) mam_endpoint_destroy(ma, ep1a);
    ntru_destroy(nb);
    free(cha);
    free(epa);
    free(ch1a);
    free(ep1a);
  }

  TEST_ASSERT(e == RC_OK);
}

static void mam_test_msg_chid_psk_one_packet_with_mac(
    sponge_t *s, void *sponge_alloc_ctx, sponge_t *(create_sponge)(void *ctx),
    void (*destroy_sponge)(void *ctx, sponge_t *), prng_t *prng_sender,
    prng_t *pb) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  mam_ialloc_t ma[1];

  sponge_t *sponge_sender = 0, *forka = 0;
  sponge_t *sponge_b = 0, *forkb = 0, *msb = 0, *wsb = 0;

  trits_t msg = trits_null(), packet = trits_null(), payload = trits_null();

  mss_mt_height_t d = 1;
  char const *cha_name_str = "CHANAME";
  mam_channel_t *cha = 0;

  mam_psk_node common_psk[1];

  mam_send_msg_context_t cfg_msg_send[1];
  mam_recv_msg_context_t cfg_msg_rcv[1];
  mam_send_packet_context_t cfg_packet_send[1];
  mam_recv_packet_context_t cfg_packet_rcv[1];

  mam_msg_checksum_t checksum; /* none=0, mac=1, mssig=2 */

  /* init alloc */
  {
    ma->create_sponge = create_sponge;
    ma->destroy_sponge = destroy_sponge;
  }
  /* create spongos */
  {
    sponge_sender = ma->create_sponge();
    forka = ma->create_sponge();

    msb = ma->create_sponge();
    wsb = ma->create_sponge();
    sponge_b = ma->create_sponge();
    forkb = ma->create_sponge();
  }
  /* init rng */
  {
    MAM2_TRITS_DEF0(key, MAM2_PRNG_KEY_SIZE);
    key = MAM2_TRITS_INIT(key, MAM2_PRNG_KEY_SIZE);

    trits_from_str(key,
                   "SENDERPRNGKEYASENDERPRNGKEY"
                   "SENDERPRNGKEYASENDERPRNGKEY"
                   "SENDERPRNGKEYASENDERPRNGKEY");
    prng_init(prng_sender, prng_sender->sponge, key);

    trits_from_str(key,
                   "RECIPIPRNGKEYBRECIPIPRNGKEY"
                   "RECIPIPRNGKEYBRECIPIPRNGKEY"
                   "RECIPIPRNGKEYBRECIPIPRNGKEY");
    prng_init(pb, pb->sponge, key);
  }

  cha = test_create_channel(cha_name_str, prng_sender, d);
  test_gen_psk(common_psk, pb, "PSKIDBPSKIDBPSKIDBPSKIDBPSK", "PSKBNONCE");

  checksum = mam_msg_checksum_mac;
  /* none=0, mac=1, mssig=2 */
  { /* init send msg context */
    mam_send_msg_context_t *cfg = cfg_msg_send;

    test_init_send_msg_cfg(cfg_msg_send, common_psk, prng_sender, cha,
                           sponge_sender, forka);

    trits_from_str(mam_send_msg_cfg_nonce(cfg), "SENDERNONCEAAAAASENDERNONCE");

    /* init recv msg context */
    {
      mam_recv_msg_context_t *cfg = cfg_msg_rcv;

      cfg->ma = ma;
      cfg->pubkey = -1;
      cfg->spongos->sponge = sponge_b;
      cfg->fork->sponge = forkb;
      cfg->spongos_mss->sponge = msb;
      cfg->spongos_wots->sponge = wsb;
      cfg->spongos_ntru->sponge = NULL;

      cfg->psk = &common_psk->info;
      cfg->ntru = 0;

      trits_copy(mam_channel_id(cha), mam_recv_msg_cfg_chid(cfg));
    }

    /* send/recv msg */
    {
      size_t sz;
      sz = mam_send_msg_size(cfg_msg_send);
      msg = trits_alloc(sz);
      TEST_ASSERT(!trits_is_null(msg));

      mam_send_msg(cfg_msg_send, &msg);
      TEST_ASSERT(trits_is_empty(msg));
      msg = trits_pickup(msg, sz);
      TEST_ASSERT(RC_OK == mam_recv_msg(cfg_msg_rcv, &msg));
      TEST_ASSERT(trits_is_empty(msg));
    }

    /* init send packet context */
    {
      mam_send_packet_context_t *cfg = cfg_packet_send;

      cfg->spongos->sponge = sponge_sender;
      cfg->spongos->pos = cfg_msg_send->spongos->pos;
      cfg->ord = 0;
      cfg->checksum = checksum;
      cfg->mss = NULL;
    }
    /* init recv packet context */
    {
      mam_recv_packet_context_t *cfg = cfg_packet_rcv;

      cfg->ma = ma;
      cfg->spongos->sponge = sponge_b;
      cfg->spongos->pos = cfg_msg_rcv->spongos->pos;
      cfg->ord = -1;
      cfg->pk = trits_null();
      TEST_ASSERT(mam_msg_pubkey_chid == cfg_msg_rcv->pubkey);
      cfg->pk = mam_recv_msg_cfg_chid(cfg_msg_rcv);
      cfg->ms->sponge = msb;
      cfg->ws->sponge = wsb;
    }

    /* send/recv packet */
    {
      size_t sz;
      char const *payload_str = "PAYLOAD9999";

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
      /*trits_free(a, payload);*/

      e = mam_recv_packet(cfg_packet_rcv, &packet, &payload);
      TEST_ASSERT(RC_OK == e);
      TEST_ASSERT(trits_is_empty(packet));
      TEST_ASSERT(trits_is_empty(payload));
      payload = trits_pickup_all(payload);
      TEST_ASSERT(trits_cmp_eq_str(payload, payload_str));
    }

    /* destroy trits */
    {
      trits_free(msg);
      trits_free(packet);
      trits_free(payload);
    }
  }

  /* destroy spongos */
  {
    ma->destroy_sponge(sponge_sender);
    ma->destroy_sponge(forka);

    ma->destroy_sponge(sponge_b);
    ma->destroy_sponge(forkb);
    ma->destroy_sponge(msb);
    ma->destroy_sponge(wsb);
  }
  /* destroy channels/endpoints */
  {
    if (cha) mam_channel_destroy(ma, cha);
    free(cha);
  }

  TEST_ASSERT(e == RC_OK);
}

static void mam_test_msg_epid_ntru_one_packet_with_sig(
    sponge_t *s, void *sponge_alloc_ctx, sponge_t *(create_sponge)(void *ctx),
    void (*destroy_sponge)(void *ctx, sponge_t *), prng_t *pa, prng_t *pb) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  mam_ialloc_t ma[1];

  sponge_t *sa = 0, *forka = 0, *nsa = 0;
  sponge_t *sb = 0, *forkb = 0, *msb = 0, *wsb = 0, *nsb = 0;

  trits_t msg = trits_null(), packet = trits_null(), payload = trits_null();

  mss_mt_height_t d = 1;
  char const *cha_name_str = "CHANAME";
  char const *epa_name_str = "EPANAME";
  mam_channel_t *cha = 0;
  mam_endpoint_t *epa = 0;

  ntru_t nb[1];
  mam_ntru_pk_node nbpk[1];

  mam_send_msg_context_t cfg_msga[1];
  mam_recv_msg_context_t cfg_msgb[1];
  mam_send_packet_context_t cfg_packeta[1];
  mam_recv_packet_context_t cfg_packetb[1];

  mam_msg_checksum_t checksum; /* none=0, mac=1, mssig=2 */

  /* init alloc */
  {
    ma->create_sponge = create_sponge;
    ma->destroy_sponge = destroy_sponge;
  }
  /* create spongos */
  {
    sa = ma->create_sponge();
    forka = ma->create_sponge();
    nsa = ma->create_sponge();

    msb = ma->create_sponge();
    wsb = ma->create_sponge();
    nsb = ma->create_sponge();
    sb = ma->create_sponge();
    forkb = ma->create_sponge();
  }
  /* init rng */
  {
    MAM2_TRITS_DEF0(k, MAM2_PRNG_KEY_SIZE);
    k = MAM2_TRITS_INIT(k, MAM2_PRNG_KEY_SIZE);

    trits_from_str(k,
                   "SENDERPRNGKEYASENDERPRNGKEY"
                   "SENDERPRNGKEYASENDERPRNGKEY"
                   "SENDERPRNGKEYASENDERPRNGKEY");
    prng_init(pa, pa->sponge, k);

    trits_from_str(k,
                   "RECIPIPRNGKEYBRECIPIPRNGKEY"
                   "RECIPIPRNGKEYBRECIPIPRNGKEY"
                   "RECIPIPRNGKEYBRECIPIPRNGKEY");
    prng_init(pb, pb->sponge, k);
  }

  /* create channels */
  {
    trits_t cha_name = trits_alloc(3 * strlen(cha_name_str));
    trits_from_str(cha_name, cha_name_str);

    cha = malloc(sizeof(mam_channel_t));
    TEST_ASSERT(0 != cha);
    memset(cha, 0, sizeof(mam_channel_t));
    e = mam_channel_create(ma, pa, d, cha_name, cha);
    TEST_ASSERT(RC_OK == e);

    /* create endpoints */
    if (1) {
      trits_t epa_name = trits_alloc(3 * strlen(epa_name_str));
      trits_from_str(epa_name, epa_name_str);

      epa = malloc(sizeof(mam_endpoint_t));
      TEST_ASSERT(0 != epa);
      memset(epa, 0, sizeof(mam_endpoint_t));
      e = mam_endpoint_create(ma, pa, d, cha_name, epa_name, epa);
      TEST_ASSERT(RC_OK == e);
      trits_free(epa_name);
    }
    trits_free(cha_name);
  }
  /* gen recipient'spongos ntru keys */
  {
    MAM2_TRITS_DEF0(N, 30);
    N = MAM2_TRITS_INIT(N, 30);
    trits_from_str(N, "NTRUBNONCE");

    e = ntru_create(nb);
    ntru_gen(nb, pb, N, mam_ntru_pk_trits(&nbpk->info));
    TEST_ASSERT(RC_OK == e);
  }

  checksum = mam_msg_checksum_mssig;
  /* none=0, mac=1, mssig=2 */
  {/* init send msg context */
   {mam_send_msg_context_t *cfg = cfg_msga;

  cfg->ma = 0;
  cfg->spongos->sponge = sa;
  cfg->fork->sponge = forka;
  cfg->prng = pa;
  cfg->rng = pa;
  cfg->ns->sponge = nsa;

  cfg->ch = cha;
  cfg->ch1 = 0;
  cfg->ep = 0;
  cfg->ep1 = 0;
  cfg->ep = epa;

  cfg->key_plain = 0;
  cfg->psks.begin = 0;
  cfg->psks.end = 0;
  cfg->ntru_pks.begin = 0;
  cfg->ntru_pks.end = 0;
  {
    nbpk->prev = nbpk->next = 0;
    mam_list_insert_end(cfg->ntru_pks, nbpk);
  }

  trits_from_str(mam_send_msg_cfg_nonce(cfg), "SENDERNONCEAAAAASENDERNONCE");
}
/* init recv msg context */
{
  mam_recv_msg_context_t *cfg = cfg_msgb;

  cfg->ma = ma;
  cfg->pubkey = -1;
  cfg->spongos->sponge = sb;
  cfg->fork->sponge = forkb;
  cfg->spongos_mss->sponge = msb;
  cfg->spongos_wots->sponge = wsb;
  cfg->spongos_ntru->sponge = nsb;

  cfg->psk = 0;
  cfg->ntru = nb;

  trits_copy(mam_channel_id(cha), mam_recv_msg_cfg_chid(cfg));
}

/* send/recv msg */
{
  size_t sz;
  sz = mam_send_msg_size(cfg_msga);
  msg = trits_alloc(sz);
  TEST_ASSERT(!trits_is_null(msg));

  mam_send_msg(cfg_msga, &msg);
  TEST_ASSERT(trits_is_empty(msg));
  msg = trits_pickup(msg, sz);

  e = mam_recv_msg(cfg_msgb, &msg);
  TEST_ASSERT(RC_OK == e);
  TEST_ASSERT(trits_is_empty(msg));
}

/* init send packet context */
{
  mam_send_packet_context_t *cfg = cfg_packeta;

  cfg->spongos->sponge = sa;
  cfg->spongos->pos = cfg_msga->spongos->pos;
  cfg->ord = 0;
  cfg->checksum = checksum;
  cfg->mss = 0;
  { cfg->mss = epa->m; }
}
/* init recv packet context */
{
  mam_recv_packet_context_t *cfg = cfg_packetb;

  cfg->ma = ma;
  cfg->spongos->sponge = sb;
  cfg->spongos->pos = cfg_msgb->spongos->pos;
  cfg->ord = -1;
  cfg->pk = trits_null();
  TEST_ASSERT(mam_msg_pubkey_epid == cfg_msgb->pubkey);
  cfg->pk = mam_recv_msg_cfg_epid(cfg_msgb);
  cfg->ms->sponge = msb;
  cfg->ws->sponge = wsb;
}

/* send/recv packet */
{
  size_t sz;
  char const *payload_str = "PAYLOAD9999";

  payload = trits_alloc(3 * strlen(payload_str));
  TEST_ASSERT(!trits_is_null(payload));
  trits_from_str(payload, payload_str);

  sz = mam_send_packet_size(cfg_packeta, trits_size(payload));
  packet = trits_alloc(sz);
  TEST_ASSERT(!trits_is_null(packet));

  mam_send_packet(cfg_packeta, payload, &packet);
  TEST_ASSERT(trits_is_empty(packet));
  packet = trits_pickup(packet, sz);
  trits_set_zero(payload);
  /*trits_free(a, payload);*/

  e = mam_recv_packet(cfg_packetb, &packet, &payload);
  TEST_ASSERT(RC_OK == e);
  TEST_ASSERT(trits_is_empty(packet));
  TEST_ASSERT(trits_is_empty(payload));
  payload = trits_pickup_all(payload);
  TEST_ASSERT(trits_cmp_eq_str(payload, payload_str));
}

/* destroy trits */
{
  trits_free(msg);
  trits_free(packet);
  trits_free(payload);
}
}

/* destroy spongos */
{
  ma->destroy_sponge(sa);
  ma->destroy_sponge(forka);
  ma->destroy_sponge(nsa);

  ma->destroy_sponge(sb);
  ma->destroy_sponge(forkb);
  ma->destroy_sponge(msb);
  ma->destroy_sponge(wsb);
  ma->destroy_sponge(nsb);
}
/* destroy channels/endpoints */
{
  if (cha) mam_channel_destroy(ma, cha);
  if (epa) mam_endpoint_destroy(ma, epa);
  ntru_destroy(nb);
  free(epa);
  free(cha);
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

  mam_test_msg_chid_psk_one_packet_with_mac(s, NULL, test_create_sponge,
                                            test_delete_sponge, pa, pb);

  mam_test_msg_epid_ntru_one_packet_with_sig(s, NULL, test_create_sponge,
                                             test_delete_sponge, pa, pb);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(mam_test);
  return UNITY_END();
}
