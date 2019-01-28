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
#ifndef __MAM_V2_MAM_MAM_H__
#define __MAM_V2_MAM_MAM_H__

#include "common/errors.h"
#include "mam/v2/defs.h"
#include "mam/v2/mam/mam_ntru_pk_t_set.h"
#include "mam/v2/mam/mam_pre_shared_key_t_set.h"
#include "mam/v2/mam/mam_types.h"
#include "mam/v2/mss/mss.h"
#include "mam/v2/ntru/ntru.h"
#include "mam/v2/prng/prng.h"
#include "mam/v2/sponge/sponge.h"
#include "mam/v2/trits/trits.h"
#include "mam/v2/wots/wots.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mam_ialloc_s {
  sponge_t *(*create_sponge)(); /*!< Allocator for sponge interfaces used by
                                              channels/endpoints (WOTS, PB3
                                   sponge, PB3 fork sponge). */
  void (*destroy_sponge)(sponge_t *); /*!< Deallocator. */
} mam_ialloc_t;

retcode_t mam_mss_create(mam_ialloc_t *ma, mss_t *mss, prng_t *p,
                         mss_mt_height_t height, trits_t nonce1,
                         trits_t nonce2);

void mam_mss_destroy(mam_ialloc_t *ma, mss_t *m);

#define MAM2_CHANNEL_ID_SIZE MAM2_MSS_MT_HASH_SIZE
typedef trit_t chid_t[MAM2_CHANNEL_ID_SIZE];
typedef struct mam_channel_s {
  mss_t mss[1]; /*!< MSS instance. */
  chid_t id;    /*!< MSS public key. */
} mam_channel_t;
trits_t mam_channel_id(mam_channel_t *ch);
trits_t mam_channel_name(mam_channel_t *ch);

/*
\brief Allocate memory for internal objects,
  and generate MSS public key.
*/
retcode_t mam_channel_create(mam_ialloc_t *ma, /*!< [in] Allocator. */
                             prng_t *prng, /*! [in] Shared PRNG interface used
                                          to generate WOTS private keys. */
                             mss_mt_height_t d, /*!< [in] MSS MT height. */
                             trits_t ch_name,   /*!< [in] Channel name. */
                             mam_channel_t *ch  /*!< [out] Channel. */
);

/*
\brief Deallocate memory for internal objects.
\note Before destroying channel make sure to destroy
  all associated endpoints.
*/
void mam_channel_destroy(mam_ialloc_t *ma, /*!< [in] Allocator. */
                         mam_channel_t *ch /*!< [out] Channel. */
);

/*
TODO: channel serialization
mam_channel_save, mam_channel_load
*/

#define MAM2_ENDPOINT_ID_SIZE MAM2_MSS_MT_HASH_SIZE
typedef trit_t epid_t[MAM2_MSS_MT_HASH_SIZE];
typedef struct mam_endpoint_s {
  mss_t mss[1]; /*!< MSS instance. */
  epid_t id;    /*!< MSS public key. */
} mam_endpoint_t;
trits_t mam_endpoint_id(mam_endpoint_t *ep);
trits_t mam_endpoint_chname(mam_endpoint_t *ep);
trits_t mam_endpoint_name(mam_endpoint_t *ep);

/*
\brief Allocate memory for internal objects,
  and generate MSS public key.
*/
retcode_t mam_endpoint_create(mam_ialloc_t *ma, /*!< [in] Allocator. */
                              prng_t *p, /*! [in] Shared PRNG interface used to
                                           generate WOTS private keys. */
                              mss_mt_height_t d, /*!< [in] MSS MT height. */
                              trits_t ch_name,   /*!< [in] Channel name. */
                              trits_t ep_name,   /*!< [in] Endpoint name. */
                              mam_endpoint_t *ep /*!< [out] Endpoint. */
);

/* \brief Deallocate memory for internal objects. */
void mam_endpoint_destroy(mam_ialloc_t *ma,  /*!< [in] Allocator. */
                          mam_endpoint_t *ep /*!< [out] Endpoint. */
);

/*
TODO: endpoint serialization
mam_endpoint_save, mam_endpoint_load
*/

trits_t mam_psk_id(mam_pre_shared_key_t *p);
trits_t mam_psk_trits(mam_pre_shared_key_t *p);

trits_t mam_ntru_pk_id(mam_ntru_pk_t *p);
trits_t mam_ntru_pk_trits(mam_ntru_pk_t *p);

/* Channel */

size_t mam_wrap_channel_size();
void mam_wrap_channel(spongos_t *s, trits_t *b, tryte_t ver, trits_t chid);
retcode_t mam_unwrap_channel(spongos_t *s, trits_t *b, tryte_t *ver,
                             trits_t chid);

/* Endpoint */

size_t mam_wrap_pubkey_chid_size();
void mam_wrap_pubkey_chid(spongos_t *s, trits_t *b);
retcode_t mam_unwrap_pubkey_chid(spongos_t *s, trits_t *b);

size_t mam_wrap_pubkey_epid_size();
void mam_wrap_pubkey_epid(spongos_t *s, trits_t *b, trits_t epid);
retcode_t mam_unwrap_pubkey_epid(spongos_t *s, trits_t *b, trits_t epid);

size_t mam_wrap_pubkey_chid1_size(mss_t *m);
void mam_wrap_pubkey_chid1(spongos_t *s, trits_t *b, trits_t chid1, mss_t *m);
retcode_t mam_unwrap_pubkey_chid1(spongos_t *s, trits_t *b, trits_t chid1,
                                  spongos_t *ms, spongos_t *ws, trits_t pk);

size_t mam_wrap_pubkey_epid1_size(mss_t *m);
void mam_wrap_pubkey_epid1(spongos_t *s, trits_t *b, trits_t epid1, mss_t *m);
retcode_t mam_unwrap_pubkey_epid1(spongos_t *s, trits_t *b, trits_t epid1,
                                  spongos_t *ms, spongos_t *ws, trits_t pk);

/* Header, Keyload */

size_t mam_wrap_keyload_plain_size();
void mam_wrap_keyload_plain(spongos_t *s, trits_t *b, trits_t key);
retcode_t mam_unwrap_keyload_plain(spongos_t *s, trits_t *b, trits_t key);

size_t mam_wrap_keyload_psk_size();
void mam_wrap_keyload_psk(spongos_t *s, trits_t *b, trits_t key, trits_t id,
                          trits_t psk);
retcode_t mam_unwrap_keyload_psk(spongos_t *s, trits_t *b, trits_t key,
                                 bool *key_found, trits_t id, trits_t psk);

size_t mam_wrap_keyload_ntru_size();
void mam_wrap_keyload_ntru(spongos_t *s, trits_t *b, trits_t key, trits_t pk,
                           prng_t *p, spongos_t *ns, trits_t N);
retcode_t mam_unwrap_keyload_ntru(spongos_t *s, trits_t *b, trits_t key,
                                  bool *key_found, trits_t pkid, ntru_t *n,
                                  spongos_t *ns);

/* Packet */

size_t mam_wrap_checksum_none_size();
void mam_wrap_checksum_none(spongos_t *s, trits_t *b);
retcode_t mam_unwrap_checksum_none(spongos_t *s, trits_t *b);

size_t mam_wrap_checksum_mac_size();
void mam_wrap_checksum_mac(spongos_t *s, trits_t *b);
retcode_t mam_unwrap_checksum_mac(spongos_t *s, trits_t *b);

size_t mam_wrap_checksum_mssig_size(mss_t *m);
void mam_wrap_checksum_mssig(spongos_t *s, trits_t *b, mss_t *m);
retcode_t mam_unwrap_checksum_mssig(spongos_t *s, trits_t *b, spongos_t *ms,
                                    spongos_t *ws, trits_t pk);

typedef enum mam_msg_pubkey_e {
  mam_msg_pubkey_chid = 0,
  mam_msg_pubkey_epid = 1,
  mam_msg_pubkey_chid1 = 2,
  mam_msg_pubkey_epid1 = 3,
} mam_msg_pubkey_t;

typedef enum mam_msg_keyload_e {
  mam_msg_keyload_plain = 0,
  mam_msg_keyload_psk = 1,
  mam_msg_keyload_ntru = 2,
} mam_msg_keyload_t;

typedef enum mam_msg_checksum_e {
  mam_msg_checksum_none = 0,
  mam_msg_checksum_mac = 1,
  mam_msg_checksum_mssig = 2,
} mam_msg_checksum_t;

#define MAM2_HEADER_NONCE_SIZE 81

typedef struct mam_send_msg_context_s {
  spongos_t spongos[1]; /*!< Main Spongos interface to wrap PB3 messages. */
  spongos_t fork[1];    /*!< Spongos interface for PB3 forks. */
  prng_t *prng; /*!< Shared deterministic PRNG instance to gen MSS keys. */
  prng_t *rng;  /*!< Volatile PRNG instance to generate ephemeral keys. */
  spongos_t spongos_ntru[1]; /*!< Spongos interface used by NTRU encr. */

  mam_channel_t *ch;   /*!< Current channel. */
  mam_channel_t *ch1;  /*!< New channel (may be null). */
  mam_endpoint_t *ep;  /*!< Current endpoint (may be null). */
  mam_endpoint_t *ep1; /*!< New endpoint (may be null). */

  trit_t nonce[MAM2_HEADER_NONCE_SIZE]; /*!< Message nonce, must be
                                                       unique for each key. */
  trit_t
      session_key[MAM2_SPONGE_KEY_SIZE]; /*!< Trits (memory) for session key. */
  bool key_plain;                        /*!< Include session key in plain? */
  mam_pre_shared_key_t_set_t
      pre_shared_keys; /*!< Encrypt message for these psks. */
  mam_ntru_pk_t_set_t
      ntru_public_keys; /*!< Encrypt message for these NTRU public keys. */
} mam_send_msg_context_t;

size_t mam_send_msg_size(mam_send_msg_context_t *cfg);

void mam_send_msg(mam_send_msg_context_t *cfg, trits_t *msg);

typedef struct mam_send_packet_context_s {
  spongos_t spongos[1]; /*!< Main Sponge interface */
  trint18_t ord;
  mam_msg_checksum_t checksum;
  mss_t *mss;
} mam_send_packet_context_t;

size_t mam_send_packet_size(mam_send_packet_context_t *cfg,
                            size_t payload_size);

void mam_send_packet(mam_send_packet_context_t *cfg, trits_t payload,
                     trits_t *packet);

typedef struct mam_recv_msg_context_s {
  mam_ialloc_t *allocator; /*!< Allocator. */
  spongos_t spongos[1];    /*!< Main Spongos interface */
  spongos_t fork[1];       /*!< Spongos interface for PB3 forks. */

  mam_msg_pubkey_t pubkey;
  trit_t chid[MAM2_CHANNEL_ID_SIZE];
  trit_t chid1[MAM2_CHANNEL_ID_SIZE];
  trit_t epid[MAM2_ENDPOINT_ID_SIZE];
  trit_t epid1[MAM2_ENDPOINT_ID_SIZE];
  spongos_t spongos_mss[1];  /*!< Sponge interface used by MSS layer */
  spongos_t spongos_wots[1]; /*!< Sponge interface used by WOTS layer */
  spongos_t spongos_ntru[1]; /*!< Sponge interface used by NTRU layer */
  bool ep_sig;               /*!< Signed? */
  /*TODO: check for trusted chid/epid*/
  /*TODO: handle (add to trusted list) new chid1*/

  trit_t nonce[MAM2_HEADER_NONCE_SIZE];
  trit_t key[MAM2_SPONGE_KEY_SIZE];  /*!< Trits (memory) for session key. */
  trit_t psk_id[MAM2_PSK_ID_SIZE];   /*!< Buffer to read PSK id to. */
  trit_t ntru_id[MAM2_NTRU_ID_SIZE]; /*!< Buffer to read NTRU id to. */
  mam_pre_shared_key_t *psk;         /*!< PSK to decrypt message. */
  ntru_t *ntru;                      /*!< NTRU sk to decrypt message. */
} mam_recv_msg_context_t;

retcode_t mam_recv_msg(mam_recv_msg_context_t *cfg, trits_t *msg);

typedef struct mam_recv_packet_context_s {
  mam_ialloc_t *allocator;   /*!< Allocator. */
  spongos_t spongos[1];      /*!< Main Spongos interface */
  trint18_t ord;             /*!< Packet ordinal number. */
  trits_t pk;                /*!< Channel/Endpoint id - MSS public key. */
  spongos_t spongos_mss[1];  /*!< Spongos interface used by MSS. */
  spongos_t spongos_wots[1]; /*!< Spongos interface used by WOTS. */
} mam_recv_packet_context_t;

retcode_t mam_recv_packet(mam_recv_packet_context_t *cfg, trits_t *packet,
                          trits_t *payload);

trits_t mam_send_msg_cfg_chid(mam_send_msg_context_t const *const cfg);
trits_t mam_send_msg_cfg_chid1(mam_send_msg_context_t const *const cfg);
trits_t mam_send_msg_cfg_epid(mam_send_msg_context_t const *const cfg);
trits_t mam_send_msg_cfg_epid1(mam_send_msg_context_t const *const cfg);
trits_t mam_send_msg_cfg_nonce(mam_send_msg_context_t const *const cfg);
trits_t mam_send_msg_cfg_session_key(mam_send_msg_context_t const *const cfg);

trits_t mam_recv_msg_cfg_chid(mam_recv_msg_context_t const *const cfg);
trits_t mam_recv_msg_cfg_chid1(mam_recv_msg_context_t const *const cfg);
trits_t mam_recv_msg_cfg_epid(mam_recv_msg_context_t const *const cfg);
trits_t mam_recv_msg_cfg_epid1(mam_recv_msg_context_t const *const cfg);
trits_t mam_recv_msg_cfg_nonce(mam_recv_msg_context_t const *const cfg);
trits_t mam_recv_msg_cfg_key(mam_recv_msg_context_t const *const cfg);
trits_t mam_recv_msg_cfg_psk_id(mam_recv_msg_context_t const *const cfg);
trits_t mam_recv_msg_cfg_ntru_id(mam_recv_msg_context_t const *const cfg);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_MAM_MAM_H__
