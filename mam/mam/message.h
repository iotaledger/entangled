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

typedef struct mam_msg_write_context_s {
  trit_t chid[MAM_CHANNEL_ID_SIZE];
  mam_spongos_t spongos;
  trint18_t ord;
  mam_mss_t *mss;
  trit_t mss_root[MAM_MSS_PK_SIZE];
} mam_msg_write_context_t;

typedef struct mam_msg_read_context_s {
  mam_spongos_t spongos; /*!< Main Spongos interface */
  trit_t pk[MAM_CHANNEL_ID_SIZE];
  /*TODO: check for trusted chid/epid*/
  /*TODO: handle (add to trusted list) new chid1*/
  trint18_t ord; /*!< Packet ordinal number. */
} mam_msg_read_context_t;

/**
 * Gets the header size for encoding
 *
 * @param ch - The channel[in]
 * @param ep - The endpoint[in]
 * @param ch1 - The new channel[in]
 * @param ep1 - The new endpoint[in]
 * @param psks - The set of pre-shared-keys[in]
 * @param ntru_pks - The set of the NTRU public keys[in]
 *
 * @return return the size of the header
 */
size_t mam_msg_header_size(mam_channel_t const *const ch,
                           mam_endpoint_t const *const ep,
                           mam_channel_t const *const ch1,
                           mam_endpoint_t const *const ep1,
                           mam_psk_t_set_t const psks,
                           mam_ntru_pk_t_set_t const ntru_pks);

/**
 * Writes a header into trits buffer
 *
 * @param ctx - The context[in, out]
 * @param prng - The PRNG[in]
 * @param ch - The channel[in]
 * @param ep - The endpoint[in]
 * @param ch1 - The new channel[in]
 * @param ep1 - The new endpoint[in]
 * @param msg_id - The msg id[in]
 * @param msg_type_id - The message type id[in]
 * @param psks - The set of pre-shared-keys[in]
 * @param ntru_pks - The set of the NTRU public keys[in]
 * @param msg - The buffer[out]
 *
 * @return return error code
 */
retcode_t mam_msg_write_header(
    mam_msg_write_context_t *const ctx, mam_prng_t const *const prng,
    mam_channel_t *const ch, mam_endpoint_t *const ep, mam_channel_t *const ch1,
    mam_endpoint_t *const ep1, trits_t msg_id, trint9_t msg_type_id,
    mam_psk_t_set_t const psks, mam_ntru_pk_t_set_t const ntru_pks,
    trits_t *const msg);

/**
 * Gets the packet size for encoding
 *
 * @param checksum - The checksum option[in]
 * @param mss - The mss (relevant for `mam_msg_checksum`_mssig checksum
 * option)[in]
 * @param payload_size - The size of the payload to encode into the packet[in]
 *
 * @return return the size of the packet
 */
size_t mam_msg_packet_size(mam_msg_checksum_t checksum,
                           mam_mss_t const *const mss, size_t payload_size);

/**
 * Writes a packet into a buffer
 *
 * @param ctx - The context[in, out]
 * @param checksum - The checksum option[in]
 * @param payload - The payload to encode into the packet[in]
 * @param buffer - The buffer where the packet is encoded[out]
 *
 * @return return the size of the packet
 */
retcode_t mam_msg_write_packet(mam_msg_write_context_t *const ctx,
                               mam_msg_checksum_t checksum,
                               trits_t const payload, trits_t *const buffer);

/**
 * Reads a header
 *
 * @param ctx - The context[in, out]
 * @param msg - The buffer to decode[in]
 * @param psks - The set of pre-shared-keys[in]
 * @param ntru_sks - The set of the NTRU secret keys[in]
 * @param msg_id - The msg_id[out]
 *
 * @return return error code
 */
retcode_t mam_msg_read_header(mam_msg_read_context_t *const ctx,
                              trits_t const *const msg,
                              mam_psk_t_set_t const psks,
                              mam_ntru_sk_t_set_t const ntru_sks,
                              trits_t msg_id);

/**
 * Reads a packet
 *
 * @param ctx - The context[in, out]
 * @param buffer - The buffer to decode[in]
 * @param payload - The decoded payload[out]
 *
 * @return return error code
 */
retcode_t mam_msg_read_packet(mam_msg_read_context_t *const ctx,
                              trits_t const *const buffer,
                              trits_t *const payload);

/**
 * Gets the size for serialization of mam_msg_write_context_t
 *
 * @param ctx - The context[in]
 *
 * @return return the size
 */
size_t mam_msg_write_ctx_serialized_size(
    mam_msg_write_context_t const *const ctx);

/**
 * Serializes a mam_msg_write_context_t into buffer
 *
 * @param ctx - The context[in]
 * @param buffer - The buffer[out]
 *
 * @return return error code
 */
void mam_msg_write_ctx_serialize(mam_msg_write_context_t const *const ctx,
                                 trits_t *const buffer);

/**
 * Deserializes a mam_msg_write_context_t from buffer
 *
 * @param buffer - The buffer[in]
 * @param ctx - The context[in, out]
 *
 * @return return error code
 */
retcode_t mam_msg_write_ctx_deserialize(trits_t *const buffer,
                                        mam_msg_write_context_t *const ctx);

/**
 * Gets the size for serialization of mam_msg_read_context_t
 *
 * @param ctx - The context[in]
 *
 * @return return the size
 */
size_t mam_msg_read_ctx_serialized_size(
    mam_msg_read_context_t const *const ctx);

/**
 * Serializes a mam_msg_read_context_t into buffer
 *
 * @param ctx - The context[in]
 * @param buffer - The buffer[out]
 *
 * @return return error code
 */
void mam_msg_read_ctx_serialize(mam_msg_read_context_t const *const ctx,
                                trits_t *const buffer);

/**
 * Deserializes a mam_msg_read_context_t from buffer
 *
 * @param buffer - The buffer[in]
 * @param ctx - The context[in, out]
 *
 * @return return error code
 */
retcode_t mam_msg_read_ctx_deserialize(trits_t *const buffer,
                                       mam_msg_read_context_t *const ctx);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_MAM_MESSAGE_H__
