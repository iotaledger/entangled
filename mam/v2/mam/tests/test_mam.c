/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "mam/v2/mam/mam.h"

// static bool_t mam_test_generic(isponge *s, void *sponge_alloc_ctx,
//                                isponge *(create_sponge)(void *ctx),
//                                void (*destroy_sponge)(void *ctx, isponge *),
//                                iprng *pa, iprng *pb) {
//   err_t e = err_internal_error;
//   mam_ialloc ma[1];
//   ialloc *a = sponge_alloc_ctx;
//
//   isponge *sa = 0, *forka = 0, *nsa = 0;
//   isponge *sb = 0, *forkb = 0, *msb = 0, *wsb = 0, *nsb = 0;
//
//   trits_t msg = trits_null(), packet = trits_null(), payload = trits_null();
//
//   mss_mt_height_t d = 1;
//   char const *cha_name_str = "CHANAME";
//   char const *ch1a_name_str = "CHANAME9";
//   char const *epa_name_str = "EPANAME";
//   char const *ep1a_name_str = "EPANAME9";
//   mam_channel *cha = 0, *ch1a = 0;
//   mam_endpoint *epa = 0, *ep1a = 0;
//
//   mam_psk_node pska[1], pskb[1];
//   intru nb[1];
//   mam_ntru_pk_node nbpk[1];
//
//   mam_send_msg_context cfg_msga[1];
//   mam_recv_msg_context cfg_msgb[1];
//   mam_send_packet_context cfg_packeta[1];
//   mam_recv_packet_context cfg_packetb[1];
//
//   mam_msg_pubkey_t pubkey;     /* chid=0, epid=1, chid1=2, epid1=3 */
//   mam_msg_keyload_t keyload;   /* plain=0, psk=1, ntru=2 */
//   mam_msg_checksum_t checksum; /* none=0, mac=1, mssig=2 */
//
//   /* init alloc */
//   {
//     ma->a = a;
//     ma->create_sponge = create_sponge;
//     ma->destroy_sponge = destroy_sponge;
//   }
//   /* create spongos */
//   {
//     sa = ma->create_sponge(ma->a);
//     forka = ma->create_sponge(ma->a);
//     nsa = ma->create_sponge(ma->a);
//
//     msb = ma->create_sponge(ma->a);
//     wsb = ma->create_sponge(ma->a);
//     nsb = ma->create_sponge(ma->a);
//     sb = ma->create_sponge(ma->a);
//     forkb = ma->create_sponge(ma->a);
//   }
//   /* init rng */
//   {
//     MAM2_TRITS_DEF0(k, MAM2_PRNG_KEY_SIZE);
//     k = MAM2_TRITS_INIT(k, MAM2_PRNG_KEY_SIZE);
//
//     trits_from_str(k,
//                    "SENDERPRNGKEYASENDERPRNGKEY"
//                    "SENDERPRNGKEYASENDERPRNGKEY"
//                    "SENDERPRNGKEYASENDERPRNGKEY");
//     prng_init(pa, pa->s, k);
//
//     trits_from_str(k,
//                    "RECIPIPRNGKEYBRECIPIPRNGKEY"
//                    "RECIPIPRNGKEYBRECIPIPRNGKEY"
//                    "RECIPIPRNGKEYBRECIPIPRNGKEY");
//     prng_init(pb, pb->s, k);
//   }
//
//   /* create channels */
//   {
//     trits_t cha_name = trits_alloc(a, 3 * strlen(cha_name_str));
//     trits_from_str(cha_name, cha_name_str);
//
//     cha = mam_alloc(ma->a, sizeof(mam_channel));
//     MAM2_ASSERT(0 != cha);
//     memset(cha, 0, sizeof(mam_channel));
//     e = mam_channel_create(ma, pa, d, cha_name, cha);
//     MAM2_ASSERT(err_ok == e);
//
//     /* create endpoints */
//     if (1) {
//       trits_t epa_name = trits_alloc(a, 3 * strlen(epa_name_str));
//       trits_from_str(epa_name, epa_name_str);
//
//       epa = mam_alloc(ma->a, sizeof(mam_endpoint));
//       MAM2_ASSERT(0 != epa);
//       memset(epa, 0, sizeof(mam_endpoint));
//       e = mam_endpoint_create(ma, pa, d, cha_name, epa_name, epa);
//       MAM2_ASSERT(err_ok == e);
//       trits_free(a, epa_name);
//     }
//     if (1) {
//       trits_t ep1a_name = trits_alloc(a, 3 * strlen(ep1a_name_str));
//       trits_from_str(ep1a_name, ep1a_name_str);
//
//       ep1a = mam_alloc(ma->a, sizeof(mam_endpoint));
//       MAM2_ASSERT(0 != ep1a);
//       memset(ep1a, 0, sizeof(mam_endpoint));
//       e = mam_endpoint_create(ma, pa, d, cha_name, ep1a_name, ep1a);
//       MAM2_ASSERT(err_ok == e);
//       trits_free(a, ep1a_name);
//     }
//     trits_free(a, cha_name);
//   }
//   if (1) {
//     trits_t ch1a_name = trits_alloc(a, 3 * strlen(ch1a_name_str));
//     trits_from_str(ch1a_name, ch1a_name_str);
//
//     ch1a = mam_alloc(ma->a, sizeof(mam_channel));
//     MAM2_ASSERT(0 != ch1a);
//     memset(ch1a, 0, sizeof(mam_channel));
//     e = mam_channel_create(ma, pa, d, ch1a_name, ch1a);
//     MAM2_ASSERT(err_ok == e);
//     trits_free(a, ch1a_name);
//   }
//   /* gen psk */
//   {
//     pska->prev = pska->next = 0;
//     trits_from_str(mam_psk_id(&pska->info), "PSKIDAPSKIDAPSKIDAPSKIDAPSK");
//     prng_gen_str(pa, MAM2_PRNG_DST_SECKEY, "PSKANONCE",
//                  mam_psk_trits(&pska->info));
//
//     pskb->prev = pskb->next = 0;
//     trits_from_str(mam_psk_id(&pskb->info), "PSKIDBPSKIDBPSKIDBPSKIDBPSK");
//     prng_gen_str(pb, MAM2_PRNG_DST_SECKEY, "PSKBNONCE",
//                  mam_psk_trits(&pskb->info));
//   }
//   /* gen recipient's ntru keys */
//   {
//     MAM2_TRITS_DEF0(N, 30);
//     N = MAM2_TRITS_INIT(N, 30);
//     trits_from_str(N, "NTRUBNONCE");
//
//     e = ntru_create(a, nb);
//     ntru_gen(nb, pb, N, mam_ntru_pk_trits(&nbpk->info));
//     MAM2_ASSERT(err_ok == e);
//   }
//
//   for (pubkey = 0; pubkey < 4; ++pubkey) /* chid=0, epid=1, chid1=2, epid1=3
//   */
//     for (keyload = 0; keyload < 3; ++keyload) /* plain=0, psk=1, ntru=2 */
//       for (checksum = 0; checksum < 3; ++checksum)
//       /* none=0, mac=1, mssig=2 */
//       {
//         /* init send msg context */
//         {
//           mam_send_msg_context *cfg = cfg_msga;
//
//           cfg->ma = 0;
//           cfg->s->s = sa;
//           cfg->fork->s = forka;
//           cfg->prng = pa;
//           cfg->rng = pa;
//           cfg->ns->s = nsa;
//
//           cfg->ch = cha;
//           cfg->ch1 = 0;
//           cfg->ep = 0;
//           cfg->ep1 = 0;
//           if (mam_msg_pubkey_epid == pubkey)
//             cfg->ep = epa;
//           else if (mam_msg_pubkey_chid1 == pubkey)
//             cfg->ch1 = ch1a;
//           else if (mam_msg_pubkey_epid1 == pubkey)
//             cfg->ep1 = ep1a;
//           else
//             ;
//
//           cfg->key_plain = 0;
//           cfg->psks.begin = 0;
//           cfg->psks.end = 0;
//           cfg->ntru_pks.begin = 0;
//           cfg->ntru_pks.end = 0;
//           if (mam_msg_keyload_plain == keyload)
//             cfg->key_plain = 1;
//           else if (mam_msg_keyload_psk == keyload) {
//             pska->prev = pska->next = 0;
//             pskb->prev = pskb->next = 0;
//             mam_list_insert_end(cfg->psks, pska);
//             mam_list_insert_end(cfg->psks, pskb);
//           } else if (mam_msg_keyload_ntru == keyload) {
//             nbpk->prev = nbpk->next = 0;
//             mam_list_insert_end(cfg->ntru_pks, nbpk);
//           } else
//             ;
//
//           trits_from_str(mam_send_msg_cfg_nonce(cfg),
//                          "SENDERNONCEAAAAASENDERNONCE");
//         }
//         /* init recv msg context */
//         {
//           mam_recv_msg_context *cfg = cfg_msgb;
//
//           cfg->ma = ma;
//           cfg->pubkey = -1;
//           cfg->s->s = sb;
//           cfg->fork->s = forkb;
//           cfg->ms->s = msb;
//           cfg->ws->s = wsb;
//           cfg->ns->s = nsb;
//
//           cfg->psk = &pskb->info;
//           cfg->ntru = nb;
//
//           trits_copy(mam_channel_id(cha), mam_recv_msg_cfg_chid(cfg));
//         }
//
//         /* send/recv msg */
//         {
//           size_t sz;
//           sz = mam_send_msg_size(cfg_msga);
//           msg = trits_alloc(ma->a, sz);
//           MAM2_ASSERT(!trits_is_null(msg));
//
//           mam_send_msg(cfg_msga, &msg);
//           MAM2_ASSERT(trits_is_empty(msg));
//           msg = trits_pickup(msg, sz);
//
//           e = mam_recv_msg(cfg_msgb, &msg);
//           MAM2_ASSERT(err_ok == e);
//           MAM2_ASSERT(trits_is_empty(msg));
//         }
//
//         /* init send packet context */
//         {
//           mam_send_packet_context *cfg = cfg_packeta;
//
//           cfg->s->s = sa;
//           cfg->s->pos = cfg_msga->s->pos;
//           cfg->ord = 0;
//           cfg->checksum = checksum;
//           cfg->m = 0;
//           if (mam_msg_checksum_mssig == cfg->checksum) {
//             if (mam_msg_pubkey_chid == pubkey)
//               cfg->m = cha->m;
//             else if (mam_msg_pubkey_epid == pubkey)
//               cfg->m = epa->m;
//             else if (mam_msg_pubkey_chid1 == pubkey)
//               cfg->m = ch1a->m;
//             else if (mam_msg_pubkey_epid1 == pubkey)
//               cfg->m = ep1a->m;
//             else
//               ;
//           }
//         }
//         /* init recv packet context */
//         {
//           mam_recv_packet_context *cfg = cfg_packetb;
//
//           cfg->ma = ma;
//           cfg->s->s = sb;
//           cfg->s->pos = cfg_msgb->s->pos;
//           cfg->ord = -1;
//           cfg->pk = trits_null();
//           if (mam_msg_pubkey_chid == cfg_msgb->pubkey)
//             cfg->pk = mam_recv_msg_cfg_chid(cfg_msgb);
//           else if (mam_msg_pubkey_epid == cfg_msgb->pubkey)
//             cfg->pk = mam_recv_msg_cfg_epid(cfg_msgb);
//           else if (mam_msg_pubkey_chid1 == cfg_msgb->pubkey)
//             cfg->pk = mam_recv_msg_cfg_chid1(cfg_msgb);
//           else if (mam_msg_pubkey_epid1 == cfg_msgb->pubkey)
//             cfg->pk = mam_recv_msg_cfg_chid1(cfg_msgb);
//           else
//             ;
//           cfg->ms->s = msb;
//           cfg->ws->s = wsb;
//         }
//
//         /* send/recv packet */
//         {
//           size_t sz;
//           char const *payload_str = "PAYLOAD9999";
//
//           payload = trits_alloc(a, 3 * strlen(payload_str));
//           MAM2_ASSERT(!trits_is_null(payload));
//           trits_from_str(payload, payload_str);
//
//           sz = mam_send_packet_size(cfg_packeta, trits_size(payload));
//           packet = trits_alloc(ma->a, sz);
//           MAM2_ASSERT(!trits_is_null(packet));
//
//           mam_send_packet(cfg_packeta, payload, &packet);
//           MAM2_ASSERT(trits_is_empty(packet));
//           packet = trits_pickup(packet, sz);
//           trits_set_zero(payload);
//           /*trits_free(a, payload);*/
//
//           e = mam_recv_packet(cfg_packetb, &packet, &payload);
//           MAM2_ASSERT(err_ok == e);
//           MAM2_ASSERT(trits_is_empty(packet));
//           MAM2_ASSERT(trits_is_empty(payload));
//           payload = trits_pickup_all(payload);
//           MAM2_ASSERT(trits_cmp_eq_str(payload, payload_str));
//         }
//
//         /* destroy trits */
//         {
//           trits_free(a, msg);
//           trits_free(a, packet);
//           trits_free(a, payload);
//         }
//       }
//
//   /* destroy spongos */
//   {
//     ma->destroy_sponge(ma->a, sa);
//     ma->destroy_sponge(ma->a, forka);
//     ma->destroy_sponge(ma->a, nsa);
//
//     ma->destroy_sponge(ma->a, sb);
//     ma->destroy_sponge(ma->a, forkb);
//     ma->destroy_sponge(ma->a, msb);
//     ma->destroy_sponge(ma->a, wsb);
//     ma->destroy_sponge(ma->a, nsb);
//   }
//   /* destroy channels/endpoints */
//   {
//     if (cha) mam_channel_destroy(ma, cha);
//     if (ch1a) mam_channel_destroy(ma, ch1a);
//     if (epa) mam_endpoint_destroy(ma, epa);
//     if (ep1a) mam_endpoint_destroy(ma, ep1a);
//   }
//
//   return e == err_ok;
// }
//
// static bool_t mam_test_msg_chid_psk_one_packet_with_mac(
//     isponge *s, void *sponge_alloc_ctx, isponge *(create_sponge)(void *ctx),
//     void (*destroy_sponge)(void *ctx, isponge *), iprng *pa, iprng *pb) {
//   err_t e = err_internal_error;
//   mam_ialloc ma[1];
//   ialloc *a = sponge_alloc_ctx;
//
//   isponge *sa = 0, *forka = 0;
//   isponge *sb = 0, *forkb = 0, *msb = 0, *wsb = 0;
//
//   trits_t msg = trits_null(), packet = trits_null(), payload = trits_null();
//
//   mss_mt_height_t d = 1;
//   char const *cha_name_str = "CHANAME";
//   mam_channel *cha = 0;
//
//   mam_psk_node pska[1], pskb[1];
//
//   mam_send_msg_context cfg_msga[1];
//   mam_recv_msg_context cfg_msgb[1];
//   mam_send_packet_context cfg_packeta[1];
//   mam_recv_packet_context cfg_packetb[1];
//
//   mam_msg_checksum_t checksum; /* none=0, mac=1, mssig=2 */
//
//   /* init alloc */
//   {
//     ma->a = a;
//     ma->create_sponge = create_sponge;
//     ma->destroy_sponge = destroy_sponge;
//   }
//   /* create spongos */
//   {
//     sa = ma->create_sponge(ma->a);
//     forka = ma->create_sponge(ma->a);
//
//     msb = ma->create_sponge(ma->a);
//     wsb = ma->create_sponge(ma->a);
//     sb = ma->create_sponge(ma->a);
//     forkb = ma->create_sponge(ma->a);
//   }
//   /* init rng */
//   {
//     MAM2_TRITS_DEF0(k, MAM2_PRNG_KEY_SIZE);
//     k = MAM2_TRITS_INIT(k, MAM2_PRNG_KEY_SIZE);
//
//     trits_from_str(k,
//                    "SENDERPRNGKEYASENDERPRNGKEY"
//                    "SENDERPRNGKEYASENDERPRNGKEY"
//                    "SENDERPRNGKEYASENDERPRNGKEY");
//     prng_init(pa, pa->s, k);
//
//     trits_from_str(k,
//                    "RECIPIPRNGKEYBRECIPIPRNGKEY"
//                    "RECIPIPRNGKEYBRECIPIPRNGKEY"
//                    "RECIPIPRNGKEYBRECIPIPRNGKEY");
//     prng_init(pb, pb->s, k);
//   }
//
//   /* create channels */
//   {
//     trits_t cha_name = trits_alloc(a, 3 * strlen(cha_name_str));
//     trits_from_str(cha_name, cha_name_str);
//
//     cha = mam_alloc(ma->a, sizeof(mam_channel));
//     MAM2_ASSERT(0 != cha);
//     memset(cha, 0, sizeof(mam_channel));
//     e = mam_channel_create(ma, pa, d, cha_name, cha);
//     MAM2_ASSERT(err_ok == e);
//
//     trits_free(a, cha_name);
//   }
//   /* gen psk */
//   {
//     pska->prev = pska->next = 0;
//     trits_from_str(mam_psk_id(&pska->info), "PSKIDAPSKIDAPSKIDAPSKIDAPSK");
//     prng_gen_str(pa, MAM2_PRNG_DST_SECKEY, "PSKANONCE",
//                  mam_psk_trits(&pska->info));
//
//     pskb->prev = pskb->next = 0;
//     trits_from_str(mam_psk_id(&pskb->info), "PSKIDBPSKIDBPSKIDBPSKIDBPSK");
//     prng_gen_str(pb, MAM2_PRNG_DST_SECKEY, "PSKBNONCE",
//                  mam_psk_trits(&pskb->info));
//   }
//
//   checksum = mam_msg_checksum_mac;
//   /* none=0, mac=1, mssig=2 */
//   {/* init send msg context */
//    {mam_send_msg_context *cfg = cfg_msga;
//
//   cfg->ma = 0;
//   cfg->s->s = sa;
//   cfg->fork->s = forka;
//   cfg->prng = pa;
//   cfg->rng = pa;
//
//   cfg->ch = cha;
//   cfg->ch1 = 0;
//   cfg->ep = 0;
//   cfg->ep1 = 0;
//
//   cfg->key_plain = 0;
//   cfg->psks.begin = 0;
//   cfg->psks.end = 0;
//   cfg->ntru_pks.begin = 0;
//   cfg->ntru_pks.end = 0;
//   {
//     pska->prev = pska->next = 0;
//     pskb->prev = pskb->next = 0;
//     mam_list_insert_end(cfg->psks, pska);
//     mam_list_insert_end(cfg->psks, pskb);
//   }
//
//   trits_from_str(mam_send_msg_cfg_nonce(cfg), "SENDERNONCEAAAAASENDERNONCE");
// }
// /* init recv msg context */
// {
//   mam_recv_msg_context *cfg = cfg_msgb;
//
//   cfg->ma = ma;
//   cfg->pubkey = -1;
//   cfg->s->s = sb;
//   cfg->fork->s = forkb;
//   cfg->ms->s = msb;
//   cfg->ws->s = wsb;
//   cfg->ns->s = 0;
//
//   cfg->psk = &pskb->info;
//   cfg->ntru = 0;
//
//   trits_copy(mam_channel_id(cha), mam_recv_msg_cfg_chid(cfg));
// }
//
// /* send/recv msg */
// {
//   size_t sz;
//   sz = mam_send_msg_size(cfg_msga);
//   msg = trits_alloc(ma->a, sz);
//   MAM2_ASSERT(!trits_is_null(msg));
//
//   mam_send_msg(cfg_msga, &msg);
//   MAM2_ASSERT(trits_is_empty(msg));
//   msg = trits_pickup(msg, sz);
//
//   e = mam_recv_msg(cfg_msgb, &msg);
//   MAM2_ASSERT(err_ok == e);
//   MAM2_ASSERT(trits_is_empty(msg));
// }
//
// /* init send packet context */
// {
//   mam_send_packet_context *cfg = cfg_packeta;
//
//   cfg->s->s = sa;
//   cfg->s->pos = cfg_msga->s->pos;
//   cfg->ord = 0;
//   cfg->checksum = checksum;
//   cfg->m = 0;
// }
// /* init recv packet context */
// {
//   mam_recv_packet_context *cfg = cfg_packetb;
//
//   cfg->ma = ma;
//   cfg->s->s = sb;
//   cfg->s->pos = cfg_msgb->s->pos;
//   cfg->ord = -1;
//   cfg->pk = trits_null();
//   MAM2_ASSERT(mam_msg_pubkey_chid == cfg_msgb->pubkey);
//   cfg->pk = mam_recv_msg_cfg_chid(cfg_msgb);
//   cfg->ms->s = msb;
//   cfg->ws->s = wsb;
// }
//
// /* send/recv packet */
// {
//   size_t sz;
//   char const *payload_str = "PAYLOAD9999";
//
//   payload = trits_alloc(a, 3 * strlen(payload_str));
//   MAM2_ASSERT(!trits_is_null(payload));
//   trits_from_str(payload, payload_str);
//
//   sz = mam_send_packet_size(cfg_packeta, trits_size(payload));
//   packet = trits_alloc(ma->a, sz);
//   MAM2_ASSERT(!trits_is_null(packet));
//
//   mam_send_packet(cfg_packeta, payload, &packet);
//   MAM2_ASSERT(trits_is_empty(packet));
//   packet = trits_pickup(packet, sz);
//   trits_set_zero(payload);
//   /*trits_free(a, payload);*/
//
//   e = mam_recv_packet(cfg_packetb, &packet, &payload);
//   MAM2_ASSERT(err_ok == e);
//   MAM2_ASSERT(trits_is_empty(packet));
//   MAM2_ASSERT(trits_is_empty(payload));
//   payload = trits_pickup_all(payload);
//   MAM2_ASSERT(trits_cmp_eq_str(payload, payload_str));
// }
//
// /* destroy trits */
// {
//   trits_free(a, msg);
//   trits_free(a, packet);
//   trits_free(a, payload);
// }
// }
//
// /* destroy spongos */
// {
//   ma->destroy_sponge(ma->a, sa);
//   ma->destroy_sponge(ma->a, forka);
//
//   ma->destroy_sponge(ma->a, sb);
//   ma->destroy_sponge(ma->a, forkb);
//   ma->destroy_sponge(ma->a, msb);
//   ma->destroy_sponge(ma->a, wsb);
// }
// /* destroy channels/endpoints */
// {
//   if (cha) mam_channel_destroy(ma, cha);
// }
//
// return e == err_ok;
// }
//
// static bool_t mam_test_msg_epid_ntru_one_packet_with_sig(
//     isponge *s, void *sponge_alloc_ctx, isponge *(create_sponge)(void *ctx),
//     void (*destroy_sponge)(void *ctx, isponge *), iprng *pa, iprng *pb) {
//   err_t e = err_internal_error;
//   mam_ialloc ma[1];
//   ialloc *a = sponge_alloc_ctx;
//
//   isponge *sa = 0, *forka = 0, *nsa = 0;
//   isponge *sb = 0, *forkb = 0, *msb = 0, *wsb = 0, *nsb = 0;
//
//   trits_t msg = trits_null(), packet = trits_null(), payload = trits_null();
//
//   mss_mt_height_t d = 1;
//   char const *cha_name_str = "CHANAME";
//   char const *epa_name_str = "EPANAME";
//   mam_channel *cha = 0;
//   mam_endpoint *epa = 0;
//
//   intru nb[1];
//   mam_ntru_pk_node nbpk[1];
//
//   mam_send_msg_context cfg_msga[1];
//   mam_recv_msg_context cfg_msgb[1];
//   mam_send_packet_context cfg_packeta[1];
//   mam_recv_packet_context cfg_packetb[1];
//
//   mam_msg_checksum_t checksum; /* none=0, mac=1, mssig=2 */
//
//   /* init alloc */
//   {
//     ma->a = a;
//     ma->create_sponge = create_sponge;
//     ma->destroy_sponge = destroy_sponge;
//   }
//   /* create spongos */
//   {
//     sa = ma->create_sponge(ma->a);
//     forka = ma->create_sponge(ma->a);
//     nsa = ma->create_sponge(ma->a);
//
//     msb = ma->create_sponge(ma->a);
//     wsb = ma->create_sponge(ma->a);
//     nsb = ma->create_sponge(ma->a);
//     sb = ma->create_sponge(ma->a);
//     forkb = ma->create_sponge(ma->a);
//   }
//   /* init rng */
//   {
//     MAM2_TRITS_DEF0(k, MAM2_PRNG_KEY_SIZE);
//     k = MAM2_TRITS_INIT(k, MAM2_PRNG_KEY_SIZE);
//
//     trits_from_str(k,
//                    "SENDERPRNGKEYASENDERPRNGKEY"
//                    "SENDERPRNGKEYASENDERPRNGKEY"
//                    "SENDERPRNGKEYASENDERPRNGKEY");
//     prng_init(pa, pa->s, k);
//
//     trits_from_str(k,
//                    "RECIPIPRNGKEYBRECIPIPRNGKEY"
//                    "RECIPIPRNGKEYBRECIPIPRNGKEY"
//                    "RECIPIPRNGKEYBRECIPIPRNGKEY");
//     prng_init(pb, pb->s, k);
//   }
//
//   /* create channels */
//   {
//     trits_t cha_name = trits_alloc(a, 3 * strlen(cha_name_str));
//     trits_from_str(cha_name, cha_name_str);
//
//     cha = mam_alloc(ma->a, sizeof(mam_channel));
//     MAM2_ASSERT(0 != cha);
//     memset(cha, 0, sizeof(mam_channel));
//     e = mam_channel_create(ma, pa, d, cha_name, cha);
//     MAM2_ASSERT(err_ok == e);
//
//     /* create endpoints */
//     if (1) {
//       trits_t epa_name = trits_alloc(a, 3 * strlen(epa_name_str));
//       trits_from_str(epa_name, epa_name_str);
//
//       epa = mam_alloc(ma->a, sizeof(mam_endpoint));
//       MAM2_ASSERT(0 != epa);
//       memset(epa, 0, sizeof(mam_endpoint));
//       e = mam_endpoint_create(ma, pa, d, cha_name, epa_name, epa);
//       MAM2_ASSERT(err_ok == e);
//       trits_free(a, epa_name);
//     }
//     trits_free(a, cha_name);
//   }
//   /* gen recipient's ntru keys */
//   {
//     MAM2_TRITS_DEF0(N, 30);
//     N = MAM2_TRITS_INIT(N, 30);
//     trits_from_str(N, "NTRUBNONCE");
//
//     e = ntru_create(a, nb);
//     ntru_gen(nb, pb, N, mam_ntru_pk_trits(&nbpk->info));
//     MAM2_ASSERT(err_ok == e);
//   }
//
//   checksum = mam_msg_checksum_mssig;
//   /* none=0, mac=1, mssig=2 */
//   {/* init send msg context */
//    {mam_send_msg_context *cfg = cfg_msga;
//
//   cfg->ma = 0;
//   cfg->s->s = sa;
//   cfg->fork->s = forka;
//   cfg->prng = pa;
//   cfg->rng = pa;
//   cfg->ns->s = nsa;
//
//   cfg->ch = cha;
//   cfg->ch1 = 0;
//   cfg->ep = 0;
//   cfg->ep1 = 0;
//   cfg->ep = epa;
//
//   cfg->key_plain = 0;
//   cfg->psks.begin = 0;
//   cfg->psks.end = 0;
//   cfg->ntru_pks.begin = 0;
//   cfg->ntru_pks.end = 0;
//   {
//     nbpk->prev = nbpk->next = 0;
//     mam_list_insert_end(cfg->ntru_pks, nbpk);
//   }
//
//   trits_from_str(mam_send_msg_cfg_nonce(cfg), "SENDERNONCEAAAAASENDERNONCE");
// }
// /* init recv msg context */
// {
//   mam_recv_msg_context *cfg = cfg_msgb;
//
//   cfg->ma = ma;
//   cfg->pubkey = -1;
//   cfg->s->s = sb;
//   cfg->fork->s = forkb;
//   cfg->ms->s = msb;
//   cfg->ws->s = wsb;
//   cfg->ns->s = nsb;
//
//   cfg->psk = 0;
//   cfg->ntru = nb;
//
//   trits_copy(mam_channel_id(cha), mam_recv_msg_cfg_chid(cfg));
// }
//
// /* send/recv msg */
// {
//   size_t sz;
//   sz = mam_send_msg_size(cfg_msga);
//   msg = trits_alloc(ma->a, sz);
//   MAM2_ASSERT(!trits_is_null(msg));
//
//   mam_send_msg(cfg_msga, &msg);
//   MAM2_ASSERT(trits_is_empty(msg));
//   msg = trits_pickup(msg, sz);
//
//   e = mam_recv_msg(cfg_msgb, &msg);
//   MAM2_ASSERT(err_ok == e);
//   MAM2_ASSERT(trits_is_empty(msg));
// }
//
// /* init send packet context */
// {
//   mam_send_packet_context *cfg = cfg_packeta;
//
//   cfg->s->s = sa;
//   cfg->s->pos = cfg_msga->s->pos;
//   cfg->ord = 0;
//   cfg->checksum = checksum;
//   cfg->m = 0;
//   { cfg->m = epa->m; }
// }
// /* init recv packet context */
// {
//   mam_recv_packet_context *cfg = cfg_packetb;
//
//   cfg->ma = ma;
//   cfg->s->s = sb;
//   cfg->s->pos = cfg_msgb->s->pos;
//   cfg->ord = -1;
//   cfg->pk = trits_null();
//   MAM2_ASSERT(mam_msg_pubkey_epid == cfg_msgb->pubkey);
//   cfg->pk = mam_recv_msg_cfg_epid(cfg_msgb);
//   cfg->ms->s = msb;
//   cfg->ws->s = wsb;
// }
//
// /* send/recv packet */
// {
//   size_t sz;
//   char const *payload_str = "PAYLOAD9999";
//
//   payload = trits_alloc(a, 3 * strlen(payload_str));
//   MAM2_ASSERT(!trits_is_null(payload));
//   trits_from_str(payload, payload_str);
//
//   sz = mam_send_packet_size(cfg_packeta, trits_size(payload));
//   packet = trits_alloc(ma->a, sz);
//   MAM2_ASSERT(!trits_is_null(packet));
//
//   mam_send_packet(cfg_packeta, payload, &packet);
//   MAM2_ASSERT(trits_is_empty(packet));
//   packet = trits_pickup(packet, sz);
//   trits_set_zero(payload);
//   /*trits_free(a, payload);*/
//
//   e = mam_recv_packet(cfg_packetb, &packet, &payload);
//   MAM2_ASSERT(err_ok == e);
//   MAM2_ASSERT(trits_is_empty(packet));
//   MAM2_ASSERT(trits_is_empty(payload));
//   payload = trits_pickup_all(payload);
//   MAM2_ASSERT(trits_cmp_eq_str(payload, payload_str));
// }
//
// /* destroy trits */
// {
//   trits_free(a, msg);
//   trits_free(a, packet);
//   trits_free(a, payload);
// }
// }
//
// /* destroy spongos */
// {
//   ma->destroy_sponge(ma->a, sa);
//   ma->destroy_sponge(ma->a, forka);
//   ma->destroy_sponge(ma->a, nsa);
//
//   ma->destroy_sponge(ma->a, sb);
//   ma->destroy_sponge(ma->a, forkb);
//   ma->destroy_sponge(ma->a, msb);
//   ma->destroy_sponge(ma->a, wsb);
//   ma->destroy_sponge(ma->a, nsb);
// }
// /* destroy channels/endpoints */
// {
//   if (cha) mam_channel_destroy(ma, cha);
//   if (epa) mam_endpoint_destroy(ma, epa);
// }
//
// return e == err_ok;
// }
//
// bool_t mam_test(isponge *s, void *sponge_alloc_ctx,
//                 isponge *(create_sponge)(void *ctx),
//                 void (*destroy_sponge)(void *ctx, isponge *), iprng *pa,
//                 iprng *pb) {
//   bool_t r = 1;
//   r = mam_test_msg_chid_psk_one_packet_with_mac(
//           s, sponge_alloc_ctx, create_sponge, destroy_sponge, pa, pb) &&
//       r;
//   r = mam_test_msg_epid_ntru_one_packet_with_sig(
//           s, sponge_alloc_ctx, create_sponge, destroy_sponge, pa, pb) &&
//       r;
//   r = mam_test_generic(s, sponge_alloc_ctx, create_sponge, destroy_sponge,
//   pa,
//                        pb) &&
//       r;
//   return r;
// }

int main(void) {
  UNITY_BEGIN();

  return UNITY_END();
}
