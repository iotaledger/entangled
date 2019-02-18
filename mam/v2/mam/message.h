/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_MAM_MESSAGE_H__
#define __MAM_V2_MAM_MESSAGE_H__

#include "common/errors.h"
#include "mam/v2/defs.h"
#include "mam/v2/mam/channel.h"
#include "mam/v2/mam/endpoint.h"
#include "mam/v2/mam/mam_psk_t_set.h"
#include "mam/v2/mam/mam_types.h"
#include "mam/v2/mss/mss.h"
#include "mam/v2/ntru/mam_ntru_pk_t_set.h"
#include "mam/v2/ntru/ntru.h"
#include "mam/v2/prng/prng.h"
#include "mam/v2/sponge/sponge.h"
#include "mam/v2/trits/trits.h"
#include "mam/v2/wots/wots.h"

#define MAM2_HEADER_MSG_ID_SIZE 81

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mam_msg_pubkey_e {
  mam_msg_pubkey_chid = 0,
  mam_msg_pubkey_epid = 1,
  mam_msg_pubkey_chid1 = 2,
  mam_msg_pubkey_epid1 = 3,
} mam_msg_pubkey_t;

typedef enum mam_msg_keyload_e {
  mam_msg_keyload_psk = 1,
  mam_msg_keyload_ntru = 2,
} mam_msg_keyload_t;

typedef enum mam_msg_checksum_e {
  mam_msg_checksum_none = 0,
  mam_msg_checksum_mac = 1,
  mam_msg_checksum_mssig = 2,
} mam_msg_checksum_t;

typedef struct mam_msg_send_context_s {
  mam_spongos_t spongos;
  trint18_t ord;
  mss_t *mss;
} mam_msg_send_context_t;

size_t mam_msg_send_size(mam_channel_t *ch, mam_endpoint_t *ep,
                         mam_channel_t *ch1, mam_endpoint_t *ep1,
                         mam_psk_t_set_t psks, mam_ntru_pk_t_set_t ntru_pks);

void mam_msg_send(mam_msg_send_context_t *ctx, mam_prng_t *prng,
                  mam_channel_t *ch, mam_endpoint_t *ep, mam_channel_t *ch1,
                  mam_endpoint_t *ep1, trits_t msg_id, trint9_t msg_type_id,
                  mam_psk_t_set_t psks, mam_ntru_pk_t_set_t ntru_pks,
                  trits_t *msg);

size_t mam_msg_send_packet_size(mam_msg_checksum_t checksum, mss_t *mss,
                                size_t payload_size);

void mam_msg_send_packet(mam_msg_send_context_t *ctx,
                         mam_msg_checksum_t checksum, trits_t payload,
                         trits_t *b);

typedef struct mam_msg_recv_context_s {
  mam_spongos_t spongos[1]; /*!< Main Spongos interface */
  mam_msg_pubkey_t pubkey;
  trit_t chid[MAM2_CHANNEL_ID_SIZE];
  trit_t chid1[MAM2_CHANNEL_ID_SIZE];
  trit_t epid[MAM2_ENDPOINT_ID_SIZE];
  trit_t epid1[MAM2_ENDPOINT_ID_SIZE];
  mam_spongos_t spongos_mss[1];  /*!< Sponge interface used by MSS layer */
  mam_spongos_t spongos_wots[1]; /*!< Sponge interface used by WOTS layer */
  /*TODO: check for trusted chid/epid*/
  /*TODO: handle (add to trusted list) new chid1*/
  trit_t msg_id[MAM2_HEADER_MSG_ID_SIZE];
  trint9_t msg_type_id;
  mam_psk_t *psk;      /*!< PSK to decrypt message. */
  mam_ntru_sk_t *ntru; /*!< NTRU sk to decrypt message. */
} mam_msg_recv_context_t;

retcode_t mam_msg_recv(mam_msg_recv_context_t *cfg, trits_t *msg);

typedef struct mam_msg_recv_packet_context_s {
  mam_spongos_t spongos[1];      /*!< Main Spongos interface */
  trint18_t ord;                 /*!< Packet ordinal number. */
  trits_t pk;                    /*!< Channel/Endpoint id - MSS public key. */
  mam_spongos_t spongos_mss[1];  /*!< Spongos interface used by MSS. */
  mam_spongos_t spongos_wots[1]; /*!< Spongos interface used by WOTS. */
} mam_msg_recv_packet_context_t;

retcode_t mam_msg_recv_packet(mam_msg_recv_packet_context_t *cfg,
                              trits_t *packet, trits_t *payload);

trits_t mam_msg_recv_cfg_chid(mam_msg_recv_context_t const *const cfg);
trits_t mam_msg_recv_cfg_chid1(mam_msg_recv_context_t const *const cfg);
trits_t mam_msg_recv_cfg_epid(mam_msg_recv_context_t const *const cfg);
trits_t mam_msg_recv_cfg_epid1(mam_msg_recv_context_t const *const cfg);
trits_t mam_msg_recv_cfg_msg_id(mam_msg_recv_context_t const *const cfg);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_MAM_MESSAGE_H__
