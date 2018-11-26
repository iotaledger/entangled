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
\file mam2.h
\brief MAM2 layer.
*/
#ifndef __MAM_V2_MAM2_H__
#define __MAM_V2_MAM2_H__

#include "mam/v2/defs.h"
#include "mam/v2/list.h"
#include "mam/v2/mss.h"
#include "mam/v2/ntru.h"
#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"
#include "mam/v2/wots.h"
#include "trits.h"

typedef struct _mam2_ialloc {
  sponge_t *(*create_sponge)(); /*!< Allocator for sponge interfaces used by
                     channels/endpoints (WOTS, PB3 sponge, PB3 fork sponge). */
  void (*destroy_sponge)(sponge_t *); /*!< Deallocator. */
} mam2_ialloc;

retcode_t mam2_mss_create(mam2_ialloc *ma, mss_t *m, prng_t *p,
                          mss_mt_height_t d, trits_t N1, trits_t N2);

void mam2_mss_destroy(mam2_ialloc *ma, mss_t *m);

#define MAM2_CHANNEL_ID_SIZE MAM2_MSS_MT_HASH_SIZE
typedef trit_t chid_t[MAM2_WORDS(MAM2_CHANNEL_ID_SIZE)];
typedef struct _mam2_channel {
  mss_t m[1]; /*!< MSS instance. */
  chid_t id;  /*!< MSS public key. */
} mam2_channel;
trits_t mam2_channel_id(mam2_channel *ch);
trit_array_t mam2_channel_name(mam2_channel *ch);

def_mam2_list_node(mam2_channel, mam2_channel_node);
def_mam2_list(mam2_channel_node, mam2_channel_list);

/*
\brief Allocate memory for internal objects,
  and generate MSS public key.
*/
retcode_t mam2_channel_create(mam2_ialloc *ma, /*!< [in] Allocator. */
                              prng_t *p, /*! [in] Shared PRNG interface used to
                                            generate WOTS private keys. */
                              mss_mt_height_t d, /*!< [in] MSS MT height. */
                              trits_t ch_name,   /*!< [in] Channel name. */
                              mam2_channel *ch   /*!< [out] Channel. */
);

/*
\brief Deallocate memory for internal objects.
\note Before destroying channel make sure to destroy
  all associated endpoints.
*/
void mam2_channel_destroy(mam2_ialloc *ma, /*!< [in] Allocator. */
                          mam2_channel *ch /*!< [out] Channel. */
);

/*
TODO: channel serialization
mam2_channel_save, mam2_channel_load
*/

#define MAM2_ENDPOINT_ID_SIZE MAM2_MSS_MT_HASH_SIZE
typedef trit_t epid_t[MAM2_WORDS(MAM2_MSS_MT_HASH_SIZE)];
typedef struct _mam2_endpoint {
  mss_t m[1]; /*!< MSS instance. */
  epid_t id;  /*!< MSS public key. */
} mam2_endpoint;
trits_t mam2_endpoint_id(mam2_endpoint *ep);
trit_array_t mam2_endpoint_chname(mam2_endpoint *ep);
trit_array_t mam2_endpoint_name(mam2_endpoint *ep);

def_mam2_list_node(mam2_endpoint, mam2_endpoint_node);
def_mam2_list(mam2_endpoint_node, mam2_endpoint_list);

/*
\brief Allocate memory for internal objects,
  and generate MSS public key.
*/
retcode_t mam2_endpoint_create(mam2_ialloc *ma, /*!< [in] Allocator. */
                               prng_t *p, /*! [in] Shared PRNG interface used to
                                             generate WOTS private keys. */
                               mss_mt_height_t d, /*!< [in] MSS MT height. */
                               trits_t ch_name,   /*!< [in] Channel name. */
                               trits_t ep_name,   /*!< [in] Endpoint name. */
                               mam2_endpoint *ep  /*!< [out] Endpoint. */
);

/* \brief Deallocate memory for internal objects. */
void mam2_endpoint_destroy(mam2_ialloc *ma,  /*!< [in] Allocator. */
                           mam2_endpoint *ep /*!< [out] Endpoint. */
);

/*
TODO: endpoint serialization
mam2_endpoint_save, mam2_endpoint_load
*/

#define MAM2_PSK_ID_SIZE 81
#define MAM2_PSK_SIZE 243
/*! \brief Preshared key. */
typedef struct _mam2_psk {
  trit_t id[MAM2_WORDS(MAM2_PSK_ID_SIZE)];
  trit_t psk[MAM2_WORDS(MAM2_PSK_SIZE)];
} mam2_psk;
trits_t mam2_psk_id(mam2_psk *p);
trits_t mam2_psk_trits(mam2_psk *p);

def_mam2_list_node(mam2_psk, mam2_psk_node);
def_mam2_list(mam2_psk_node, mam2_psk_list);

/*! \brief Recipient's NTRU public key. */
typedef struct _mam2_ntru_pk {
  trit_t pk[MAM2_WORDS(MAM2_NTRU_PK_SIZE)];
} mam2_ntru_pk;
trits_t mam2_ntru_pk_id(mam2_ntru_pk *p);
trits_t mam2_ntru_pk_trits(mam2_ntru_pk *p);

def_mam2_list_node(mam2_ntru_pk, mam2_ntru_pk_node);
def_mam2_list(mam2_ntru_pk_node, mam2_ntru_pk_list);

#define MAM2_HEADER_NONCE_SIZE 81

typedef struct _mam2_send_msg_context {
  mam2_ialloc *ma; /*!< Allocator. */
  sponge_t *s;     /*!< Main Sponge interface used to wrap PB3 messages. */
  sponge_t *fork;  /*!< Sponge interface used for PB3 forks. */
  prng_t *prng; /*!< Shared deterministic PRNG instance used to gen MSS keys. */
  prng_t *rng;  /*!< Volatile PRNG instance used to generate ephemeral keys. */

  mam2_channel *ch;  /*!< Current channel. */
  mam2_channel *ch1; /*!< New channel (may be null). */
  mam2_endpoint *ep; /*!< Current endpoint (may be null). */
  bool_t ep_sig;     /*!< Need to sign Endpoint.sig? */

  trits_t nonce; /*!< Message nonce, must be unique for each key. */
  trit_t key[MAM2_WORDS(
      MAM2_SPONGE_KEY_SIZE)]; /*!< Trits (memory) for session key. */
  bool_t key_plain;           /*!< Include session key in plain? */
  mam2_psk_list psks;         /*!< Encrypt message for these psks. */
  mam2_ntru_pk_list
      ntru_pks; /*!< Encrypt message for these NTRU public keys. */
} mam2_send_msg_context;

size_t mam2_send_msg_size(mam2_send_msg_context *cfg);

retcode_t mam2_send_msg(mam2_send_msg_context *cfg, trits_t *msg);

typedef struct _mam2_send_packet_context {
  sponge_t *s; /*!< Main Sponge interface */
  trint18_t ord;
  tryte_t checksum;
  mss_t *m;
} mam2_send_packet_context;

size_t mam2_send_packet_size(mam2_send_packet_context *cfg,
                             size_t payload_size);

retcode_t mam2_send_packet(mam2_send_packet_context *cfg, trits_t payload,
                           trits_t *packet);

typedef struct _mam2_recv_msg_context {
  mam2_ialloc *ma; /*!< Allocator. */
  sponge_t *s;     /*!< Main Sponge interface */
  sponge_t *fork;  /*!< Sponge interface used for PB3 forks. */

  tryte_t pubkey;
  trit_t chid[MAM2_WORDS(MAM2_CHANNEL_ID_SIZE)];
  trit_t chid1[MAM2_WORDS(MAM2_CHANNEL_ID_SIZE)];
  trit_t epid[MAM2_WORDS(MAM2_ENDPOINT_ID_SIZE)];
  sponge_t *ms;  /*!< Sponge interface used by MSS layer */
  sponge_t *ws;  /*!< Sponge interface used by WOTS layer */
  bool_t ep_sig; /*!< Signed? */
  // TODO: check for trusted chid/epid
  // TODO: handle (add to trusted list) new chid1

  trit_t nonce[MAM2_WORDS(MAM2_HEADER_NONCE_SIZE)];
  trit_t key[MAM2_WORDS(
      MAM2_SPONGE_KEY_SIZE)]; /*!< Trits (memory) for session key. */
  trit_t psk_id[MAM2_WORDS(MAM2_PSK_ID_SIZE)]; /*!< Buffer to read PSK id to. */
  trit_t
      ntru_id[MAM2_WORDS(MAM2_NTRU_ID_SIZE)]; /*!< Buffer to read NTRU id to. */
  mam2_psk *psk;                              /*!< PSK to decrypt message. */
  intru *ntru; /*!< NTRU sk to decrypt message. */
} mam2_recv_msg_context;

retcode_t mam2_recv_msg(mam2_recv_msg_context *cfg, trits_t *msg);

typedef struct _mam2_recv_packet_context {
  mam2_ialloc *ma; /*!< Allocator. */
  sponge_t *s;     /*!< Main Sponge interface */
  trint18_t ord;   /*!< Packet ordinal number. */
  trits_t pk;      /*!< Channel/Endpoint id - MSS public key. */
  sponge_t *ms;    /*!< Sponge interface used by MSS. */
  sponge_t *ws;    /*!< Sponge interface used by WOTS. */
} mam2_recv_packet_context;

retcode_t mam2_recv_packet(mam2_recv_packet_context *cfg, trits_t *packet,
                           trits_t *payload);

#endif  // __MAM_V2_MAM2_H__
