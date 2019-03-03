/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_MAM_MESSAGE_H__
#define __MAM_MAM_MESSAGE_H__

#include "common/errors.h"
#include "mam/defs.h"
#include "mam/mam/channel.h"
#include "mam/mam/endpoint.h"
#include "mam/mss/mss.h"
#include "mam/ntru/mam_ntru_pk_t_set.h"
#include "mam/ntru/ntru.h"
#include "mam/prng/prng.h"
#include "mam/psk/mam_psk_t_set.h"
#include "mam/psk/psk.h"
#include "mam/sponge/sponge.h"
#include "mam/trits/trits.h"
#include "mam/wots/wots.h"

#define MAM_MSG_ID_SIZE 63
#define MAM_MSG_ORD_SIZE 18

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
  mam_mss_t *mss;
} mam_msg_send_context_t;

typedef struct mam_msg_recv_context_s {
  mam_spongos_t spongos; /*!< Main Spongos interface */
  trit_t pk[MAM_CHANNEL_ID_SIZE];
  /*TODO: check for trusted chid/epid*/
  /*TODO: handle (add to trusted list) new chid1*/
  trint18_t ord; /*!< Packet ordinal number. */
} mam_msg_recv_context_t;

size_t mam_msg_send_size(mam_channel_t *ch, mam_endpoint_t *ep,
                         mam_channel_t *ch1, mam_endpoint_t *ep1,
                         mam_psk_t_set_t psks, mam_ntru_pk_t_set_t ntru_pks);

void mam_msg_send(mam_msg_send_context_t *ctx, mam_prng_t *prng,
                  mam_channel_t *ch, mam_endpoint_t *ep, mam_channel_t *ch1,
                  mam_endpoint_t *ep1, trits_t msg_id, trint9_t msg_type_id,
                  mam_psk_t_set_t psks, mam_ntru_pk_t_set_t ntru_pks,
                  trits_t *msg);

size_t mam_msg_send_packet_size(mam_msg_checksum_t checksum, mam_mss_t *mss,
                                size_t payload_size);

void mam_msg_send_packet(mam_msg_send_context_t *ctx,
                         mam_msg_checksum_t checksum, trits_t payload,
                         trits_t *b);

retcode_t mam_msg_recv(mam_msg_recv_context_t *ctx, trits_t const *const msg,
                       mam_psk_t_set_t psks, mam_ntru_sk_t_set_t ntru_sks,
                       trits_t msg_id);

retcode_t mam_msg_recv_packet(mam_msg_recv_context_t *ctx, trits_t *packet,
                              trits_t *payload);

size_t mam_msg_send_ctx_serialized_size(
    mam_msg_send_context_t const *const ctx);

void mam_msg_send_ctx_serialize(mam_msg_send_context_t const *const ctx,
                                trits_t *const buffer);

retcode_t mam_msg_send_ctx_deserialize(trits_t *const buffer,
                                       mam_msg_send_context_t *const ctx);

size_t mam_msg_recv_ctx_serialized_size(
    mam_msg_recv_context_t const *const ctx);

void mam_msg_recv_ctx_serialize(mam_msg_recv_context_t const *const ctx,
                                trits_t *const buffer);

retcode_t mam_msg_recv_ctx_deserialize(trits_t *const buffer,
                                       mam_msg_recv_context_t *const ctx);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_MAM_MESSAGE_H__
