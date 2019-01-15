
/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

/*!
\file mam.h
\brief MAM2 layer.
*/
#pragma once

#include "mam/v2/defs.h"
#include "mam/v2/err.h"
#include "mam/v2/list.h"
#include "mam/v2/mss/mss.h"
#include "mam/v2/ntru/ntru.h"
#include "mam/v2/prng/prng.h"
#include "mam/v2/sponge/sponge.h"
#include "mam/v2/trits/trits.h"
#include "mam/v2/wots/wots.h"

typedef struct _mam_ialloc {
  ialloc *a; /*!< Context passed to `create_sponge` and `destroy_sponge`. */
  isponge *(*create_sponge)(
      ialloc *a); /*!< Allocator for sponge interfaces used by
                     channels/endpoints (WOTS, PB3 sponge, PB3 fork sponge). */
  void (*destroy_sponge)(ialloc *a, isponge *); /*!< Deallocator. */
} mam_ialloc;

err_t mam_mss_create(mam_ialloc *ma, imss *m, iprng *p, mss_mt_height_t d,
                     trits_t N1, trits_t N2);

void mam_mss_destroy(mam_ialloc *ma, imss *m);

#define MAM2_CHANNEL_ID_SIZE MAM2_MSS_MT_HASH_SIZE
typedef word_t chid_t[MAM2_WORDS(MAM2_CHANNEL_ID_SIZE)];
typedef struct _mam_channel {
  imss m[1]; /*!< MSS instance. */
  chid_t id; /*!< MSS public key. */
} mam_channel;
trits_t mam_channel_id(mam_channel *ch);
trits_t mam_channel_name(mam_channel *ch);

def_mam_list_node(mam_channel, mam_channel_node);
def_mam_list(mam_channel_node, mam_channel_list);

/*
\brief Allocate memory for internal objects,
  and generate MSS public key.
*/
err_t mam_channel_create(mam_ialloc *ma, /*!< [in] Allocator. */
                         iprng *p,       /*! [in] Shared PRNG interface used to
                                            generate WOTS private keys. */
                         mss_mt_height_t d, /*!< [in] MSS MT height. */
                         trits_t ch_name,   /*!< [in] Channel name. */
                         mam_channel *ch    /*!< [out] Channel. */
);

/*
\brief Deallocate memory for internal objects.
\note Before destroying channel make sure to destroy
  all associated endpoints.
*/
void mam_channel_destroy(mam_ialloc *ma, /*!< [in] Allocator. */
                         mam_channel *ch /*!< [out] Channel. */
);

/*
TODO: channel serialization
mam_channel_save, mam_channel_load
*/

#define MAM2_ENDPOINT_ID_SIZE MAM2_MSS_MT_HASH_SIZE
typedef word_t epid_t[MAM2_WORDS(MAM2_MSS_MT_HASH_SIZE)];
typedef struct _mam_endpoint {
  imss m[1]; /*!< MSS instance. */
  epid_t id; /*!< MSS public key. */
} mam_endpoint;
trits_t mam_endpoint_id(mam_endpoint *ep);
trits_t mam_endpoint_chname(mam_endpoint *ep);
trits_t mam_endpoint_name(mam_endpoint *ep);

def_mam_list_node(mam_endpoint, mam_endpoint_node);
def_mam_list(mam_endpoint_node, mam_endpoint_list);

/*
\brief Allocate memory for internal objects,
  and generate MSS public key.
*/
err_t mam_endpoint_create(mam_ialloc *ma, /*!< [in] Allocator. */
                          iprng *p,       /*! [in] Shared PRNG interface used to
                                             generate WOTS private keys. */
                          mss_mt_height_t d, /*!< [in] MSS MT height. */
                          trits_t ch_name,   /*!< [in] Channel name. */
                          trits_t ep_name,   /*!< [in] Endpoint name. */
                          mam_endpoint *ep   /*!< [out] Endpoint. */
);

/* \brief Deallocate memory for internal objects. */
void mam_endpoint_destroy(mam_ialloc *ma,  /*!< [in] Allocator. */
                          mam_endpoint *ep /*!< [out] Endpoint. */
);

/*
TODO: endpoint serialization
mam_endpoint_save, mam_endpoint_load
*/

#define MAM2_KEY_ID_SIZE 81
#define MAM2_PSK_ID_SIZE 81
#define MAM2_PSK_SIZE 243
/*! \brief Preshared key. */
typedef struct _mam_psk {
  word_t id[MAM2_WORDS(MAM2_PSK_ID_SIZE)];
  word_t psk[MAM2_WORDS(MAM2_PSK_SIZE)];
} mam_psk;
trits_t mam_psk_id(mam_psk *p);
trits_t mam_psk_trits(mam_psk *p);

def_mam_list_node(mam_psk, mam_psk_node);
def_mam_list(mam_psk_node, mam_psk_list);

/*! \brief Recipient's NTRU public key. */
typedef struct _mam_ntru_pk {
  word_t pk[MAM2_WORDS(MAM2_NTRU_PK_SIZE)];
} mam_ntru_pk;
trits_t mam_ntru_pk_id(mam_ntru_pk *p);
trits_t mam_ntru_pk_trits(mam_ntru_pk *p);

def_mam_list_node(mam_ntru_pk, mam_ntru_pk_node);
def_mam_list(mam_ntru_pk_node, mam_ntru_pk_list);

/* Channel */

size_t mam_wrap_channel_size();
void mam_wrap_channel(ispongos *s, trits_t *b, tryte_t ver, trits_t chid);
err_t mam_unwrap_channel(ispongos *s, trits_t *b, tryte_t *ver, trits_t chid);

/* Endpoint */

size_t mam_wrap_pubkey_chid_size();
void mam_wrap_pubkey_chid(ispongos *s, trits_t *b);
err_t mam_unwrap_pubkey_chid(ispongos *s, trits_t *b);

size_t mam_wrap_pubkey_epid_size();
void mam_wrap_pubkey_epid(ispongos *s, trits_t *b, trits_t epid);
err_t mam_unwrap_pubkey_epid(ispongos *s, trits_t *b, trits_t epid);

size_t mam_wrap_pubkey_chid1_size(imss *m);
void mam_wrap_pubkey_chid1(ispongos *s, trits_t *b, trits_t chid1, imss *m);
err_t mam_unwrap_pubkey_chid1(ispongos *s, trits_t *b, trits_t chid1,
                              ispongos *ms, ispongos *ws, trits_t pk);

size_t mam_wrap_pubkey_epid1_size(imss *m);
void mam_wrap_pubkey_epid1(ispongos *s, trits_t *b, trits_t epid1, imss *m);
err_t mam_unwrap_pubkey_epid1(ispongos *s, trits_t *b, trits_t epid1,
                              ispongos *ms, ispongos *ws, trits_t pk);

/* Header, Keyload */

size_t mam_wrap_keyload_plain_size();
void mam_wrap_keyload_plain(ispongos *s, trits_t *b, trits_t key);
err_t mam_unwrap_keyload_plain(ispongos *s, trits_t *b, trits_t key);

size_t mam_wrap_keyload_psk_size();
void mam_wrap_keyload_psk(ispongos *s, trits_t *b, trits_t key, trits_t id,
                          trits_t psk);
err_t mam_unwrap_keyload_psk(ispongos *s, trits_t *b, trits_t key,
                             bool_t *key_found, trits_t id, trits_t psk);

size_t mam_wrap_keyload_ntru_size();
void mam_wrap_keyload_ntru(ispongos *s, trits_t *b, trits_t key, trits_t pk,
                           iprng *p, ispongos *ns, trits_t N);
err_t mam_unwrap_keyload_ntru(ispongos *s, trits_t *b, trits_t key,
                              bool_t *key_found, trits_t pkid, intru *n,
                              ispongos *ns);

/* Packet */

size_t mam_wrap_checksum_none_size();
void mam_wrap_checksum_none(ispongos *s, trits_t *b);
err_t mam_unwrap_checksum_none(ispongos *s, trits_t *b);

size_t mam_wrap_checksum_mac_size();
void mam_wrap_checksum_mac(ispongos *s, trits_t *b);
err_t mam_unwrap_checksum_mac(ispongos *s, trits_t *b);

size_t mam_wrap_checksum_mssig_size(imss *m);
void mam_wrap_checksum_mssig(ispongos *s, trits_t *b, imss *m);
err_t mam_unwrap_checksum_mssig(ispongos *s, trits_t *b, ispongos *ms,
                                ispongos *ws, trits_t pk);

typedef enum _mam_msg_pubkey {
  mam_msg_pubkey_chid = 0,
  mam_msg_pubkey_epid = 1,
  mam_msg_pubkey_chid1 = 2,
  mam_msg_pubkey_epid1 = 3,
} mam_msg_pubkey_t;

typedef enum _mam_msg_keyload {
  mam_msg_keyload_plain = 0,
  mam_msg_keyload_psk = 1,
  mam_msg_keyload_ntru = 2,
} mam_msg_keyload_t;

typedef enum _mam_msg_checksum {
  mam_msg_checksum_none = 0,
  mam_msg_checksum_mac = 1,
  mam_msg_checksum_mssig = 2,
} mam_msg_checksum_t;

#define MAM2_HEADER_NONCE_SIZE 81

typedef struct _mam_send_msg_context {
  mam_ialloc *ma;   /*!< Allocator. */
  ispongos s[1];    /*!< Main Spongos interface to wrap PB3 messages. */
  ispongos fork[1]; /*!< Spongos interface for PB3 forks. */
  iprng *prng;      /*!< Shared deterministic PRNG instance to gen MSS keys. */
  iprng *rng;       /*!< Volatile PRNG instance to generate ephemeral keys. */
  ispongos ns[1];   /*!< Spongos interface used by NTRU encr. */

  mam_channel *ch;   /*!< Current channel. */
  mam_channel *ch1;  /*!< New channel (may be null). */
  mam_endpoint *ep;  /*!< Current endpoint (may be null). */
  mam_endpoint *ep1; /*!< New endpoint (may be null). */

  word_t nonce[MAM2_WORDS(MAM2_HEADER_NONCE_SIZE)]; /*!< Message nonce, must be
                                                       unique for each key. */
  word_t key[MAM2_WORDS(
      MAM2_SPONGE_KEY_SIZE)]; /*!< Trits (memory) for session key. */
  bool_t key_plain;           /*!< Include session key in plain? */
  mam_psk_list psks;          /*!< Encrypt message for these psks. */
  mam_ntru_pk_list ntru_pks; /*!< Encrypt message for these NTRU public keys. */
} mam_send_msg_context;

size_t mam_send_msg_size(mam_send_msg_context *cfg);

void mam_send_msg(mam_send_msg_context *cfg, trits_t *msg);

typedef struct _mam_send_packet_context {
  ispongos s[1]; /*!< Main Sponge interface */
  trint18_t ord;
  mam_msg_checksum_t checksum;
  imss *m;
} mam_send_packet_context;

size_t mam_send_packet_size(mam_send_packet_context *cfg, size_t payload_size);

void mam_send_packet(mam_send_packet_context *cfg, trits_t payload,
                     trits_t *packet);

typedef struct _mam_recv_msg_context {
  mam_ialloc *ma;   /*!< Allocator. */
  ispongos s[1];    /*!< Main Spongos interface */
  ispongos fork[1]; /*!< Spongos interface for PB3 forks. */

  mam_msg_pubkey_t pubkey;
  word_t chid[MAM2_WORDS(MAM2_CHANNEL_ID_SIZE)];
  word_t chid1[MAM2_WORDS(MAM2_CHANNEL_ID_SIZE)];
  word_t epid[MAM2_WORDS(MAM2_ENDPOINT_ID_SIZE)];
  word_t epid1[MAM2_WORDS(MAM2_ENDPOINT_ID_SIZE)];
  ispongos ms[1]; /*!< Sponge interface used by MSS layer */
  ispongos ws[1]; /*!< Sponge interface used by WOTS layer */
  ispongos ns[1]; /*!< Sponge interface used by NTRU layer */
  bool_t ep_sig;  /*!< Signed? */
  /*TODO: check for trusted chid/epid*/
  /*TODO: handle (add to trusted list) new chid1*/

  word_t nonce[MAM2_WORDS(MAM2_HEADER_NONCE_SIZE)];
  word_t key[MAM2_WORDS(
      MAM2_SPONGE_KEY_SIZE)]; /*!< Trits (memory) for session key. */
  word_t psk_id[MAM2_WORDS(MAM2_PSK_ID_SIZE)]; /*!< Buffer to read PSK id to. */
  word_t
      ntru_id[MAM2_WORDS(MAM2_NTRU_ID_SIZE)]; /*!< Buffer to read NTRU id to. */
  mam_psk *psk;                               /*!< PSK to decrypt message. */
  intru *ntru; /*!< NTRU sk to decrypt message. */
} mam_recv_msg_context;

err_t mam_recv_msg(mam_recv_msg_context *cfg, trits_t *msg);

typedef struct _mam_recv_packet_context {
  mam_ialloc *ma; /*!< Allocator. */
  ispongos s[1];  /*!< Main Spongos interface */
  trint18_t ord;  /*!< Packet ordinal number. */
  trits_t pk;     /*!< Channel/Endpoint id - MSS public key. */
  ispongos ms[1]; /*!< Spongos interface used by MSS. */
  ispongos ws[1]; /*!< Spongos interface used by WOTS. */
} mam_recv_packet_context;

err_t mam_recv_packet(mam_recv_packet_context *cfg, trits_t *packet,
                      trits_t *payload);

trits_t mam_send_msg_cfg_chid(mam_send_msg_context *cfg);
trits_t mam_send_msg_cfg_chid1(mam_send_msg_context *cfg);
trits_t mam_send_msg_cfg_epid(mam_send_msg_context *cfg);
trits_t mam_send_msg_cfg_epid1(mam_send_msg_context *cfg);
trits_t mam_send_msg_cfg_nonce(mam_send_msg_context *cfg);
trits_t mam_send_msg_cfg_key(mam_send_msg_context *cfg);

trits_t mam_recv_msg_cfg_chid(mam_recv_msg_context *cfg);
trits_t mam_recv_msg_cfg_chid1(mam_recv_msg_context *cfg);
trits_t mam_recv_msg_cfg_epid(mam_recv_msg_context *cfg);
trits_t mam_recv_msg_cfg_epid1(mam_recv_msg_context *cfg);
trits_t mam_recv_msg_cfg_nonce(mam_recv_msg_context *cfg);
trits_t mam_recv_msg_cfg_key(mam_recv_msg_context *cfg);
trits_t mam_recv_msg_cfg_psk_id(mam_recv_msg_context *cfg);
trits_t mam_recv_msg_cfg_ntru_id(mam_recv_msg_context *cfg);
