/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_API_API_H__
#define __MAM_API_API_H__

#include "common/errors.h"
#include "common/model/bundle.h"
#include "common/trinary/flex_trit.h"
#include "mam/api/trit_t_to_mam_msg_read_context_t_map.h"
#include "mam/api/trit_t_to_mam_msg_write_context_t_map.h"
#include "mam/mam/message.h"
#include "mam/ntru/ntru_types.h"
#include "mam/prng/prng_types.h"
#include "mam/psk/psk.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mam_api_s {
  mam_prng_t prng;
  mam_ntru_sk_t_set_t ntru_sks;
  mam_ntru_pk_t_set_t ntru_pks;
  mam_psk_t_set_t psks;
  trit_t_to_mam_msg_write_context_t_map_t write_ctxs;
  trit_t_to_mam_msg_read_context_t_map_t read_ctxs;
  mam_channel_t_set_t channels;
  trint18_t channel_ord;
} mam_api_t;

/**
 * Initialize an API
 *
 * @param api - The API [out]
 * @param mam_seed - The seed for PRNG initialization [in]
 *
 * @return return code
 */
retcode_t mam_api_init(mam_api_t *const api, tryte_t const *const mam_seed);

/**
 * Destroys an API
 *
 * @param api - The API [in,out]
 *
 * @return return code
 */
retcode_t mam_api_destroy(mam_api_t *const api);

/**
 * Add a NTRU secret key to api's NTRU sks set
 *
 * @param api - The API [in,out]
 * @param ntru_sk - A new ntru public key (allows for both enc/dec) [in]
 *
 * @return return code
 */
retcode_t mam_api_add_ntru_sk(mam_api_t *const api,
                              mam_ntru_sk_t const *const ntru_sk);

/**
 * Add a NTRU public key to api's NTRU pks set
 *
 * @param api - The API [in,out]
 * @param ntru_pk - A new ntru public key (allows for encryption only) [in]
 *
 * @return return code
 */
retcode_t mam_api_add_ntru_pk(mam_api_t *const api,
                              mam_ntru_pk_t const *const ntru_pk);

/**
 * Add a pre shared key to api's psks set
 *
 * @param api - The API [in,out]
 * @param psk - A new psk [in]
 *
 * @return return code
 */
retcode_t mam_api_add_psk(mam_api_t *const api, mam_psk_t const *const psk);

/**
 * Creates and adds a channel to the API
 *
 * @param api - The API [in, out]
 * @param height - The channel's MSS height [in]
 * @param channel_id - The channel id [out]
 *
 * @return return code
 */
retcode_t mam_api_create_channel(mam_api_t *const api, size_t const height,
                                 tryte_t *const channel_id);

/**
 * Gets a channel from its id
 *
 * @param api - The API [in]
 * @param channel_id - The channel id [in]
 *
 * @return a pointer to the channel or NULL if not found
 */
mam_channel_t *mam_api_get_channel(mam_api_t const *const api,
                                   tryte_t const *const channel_id);

/**
 * Creates and adds an endpoint to the API
 *
 * @param api - The API [in, out]
 * @param height - The endpoint's MSS height [in]
 * @param channel_id - The associated channel id [in]
 * @param endpoint_id - The endpoint id [out]
 *
 * @return return code
 */
retcode_t mam_api_create_endpoint(mam_api_t *const api, size_t const height,
                                  tryte_t const *const channel_id,
                                  tryte_t *const endpoint_id);

/**
 * Gets an endpoint from its id
 *
 * @param api - The API [in]
 * @param channel_id - The associated channel id [in]
 * @param endpoint_id - The endpoint id [in]
 *
 * @return a pointer to the endpoint or NULL if not found
 */
mam_endpoint_t *mam_api_get_endpoint(mam_api_t const *const api,
                                     tryte_t const *const channel_id,
                                     tryte_t const *const endpoint_id);

/**
 * Writes MAM header on a channel(keyloads (session keys) + potential packet)
 * into a bundle
 *
 * @param api - The API [in,out]
 * @param ch_id - A known channel ID [in]
 * @param psks - pre shared keys used for encrypting the session keys [in]
 * @param ntru_pks - ntru public keys used for encrypting the session keys [in]
 * @param msg_type_id - The message type [in]
 * @param bundle - The bundle that the packet will be written into [out]
 * @param msg_id - The msg_id (hashed channel_name and message index within the
 *  channel) embedded into transaction's tag (together with packet index to
 * allow Tangle lookup) [out]
 *
 * @return return code
 */
retcode_t mam_api_bundle_write_header_on_channel(
    mam_api_t *const api, tryte_t const *const ch_id, mam_psk_t_set_t psks,
    mam_ntru_pk_t_set_t ntru_pks, trint9_t msg_type_id,
    bundle_transactions_t *const bundle, trit_t *const msg_id);

/**
 * Writes MAM header on an endpoint(keyloads (session keys) + potential packet)
 * into a bundle
 *
 * @param api - The API [in,out]
 * @param ch_id - A known channel ID [in]
 * @param ep_id - A known endpoint ID [in]
 * @param psks - pre shared keys used for encrypting the session keys [in]
 * @param ntru_pks - ntru public keys used for encrypting the session keys [in]
 * @param msg_type_id - The message type [in]
 * @param bundle - The bundle that the packet will be written into [out]
 * @param msg_id - The msg_id (hashed channel_name and message index within the
 *  channel) embedded into transaction's tag (together with packet index to
 * allow Tangle lookup) [out]
 *
 * @return return code
 */
retcode_t mam_api_bundle_write_header_on_endpoint(
    mam_api_t *const api, tryte_t const *const ch_id,
    tryte_t const *const ep_id, mam_psk_t_set_t psks,
    mam_ntru_pk_t_set_t ntru_pks, trint9_t msg_type_id,
    bundle_transactions_t *const bundle, trit_t *const msg_id);

/**
 * Writes an announcement of a new channel (keyloads (session keys) +
 * potential packet) into a bundle
 *
 * @param api - The API [in,out]
 * @param ch_id - A known channel ID [in]
 * @param ch1_id - The new channel ID [in]
 * @param psks - pre shared keys used for encrypting the session keys [in]
 * @param ntru_pks - ntru public keys used for encrypting the session keys [in]
 * @param msg_type_id - The message type [in]
 * @param bundle - The bundle that the packet will be written into [out]
 * @param msg_id - The msg_id (hashed channel_name and message index within the
 *  channel) embedded into transaction's tag (together with packet index to
 * allow Tangle lookup) [out]
 *
 * @return return code
 */
retcode_t mam_api_bundle_announce_new_channel(
    mam_api_t *const api, tryte_t const *const ch_id,
    tryte_t const *const ch1_id, mam_psk_t_set_t psks,
    mam_ntru_pk_t_set_t ntru_pks, trint9_t msg_type_id,
    bundle_transactions_t *const bundle, trit_t *const msg_id);

/**
 * Writes an announcement of a new endpoint (keyloads (session keys) +
 * potential packet) into a bundle
 *
 * @param api - The API [in,out]
 * @param ch_id - A known channel ID [in]
 * @param ep1_id - The new endpoint ID [in]
 * @param psks - pre shared keys used for encrypting the session keys [in]
 * @param ntru_pks - ntru public keys used for encrypting the session keys [in]
 * @param msg_type_id - The message type [in]
 * @param bundle - The bundle that the packet will be written into [out]
 * @param msg_id - The msg_id (hashed channel_name and message index within the
 *  channel) embedded into transaction's tag (together with packet index to
 * allow Tangle lookup) [out]
 *
 * @return return code
 */
retcode_t mam_api_bundle_announce_new_endpoint(
    mam_api_t *const api, tryte_t const *const ch_id,
    tryte_t const *const ep1_id, mam_psk_t_set_t psks,
    mam_ntru_pk_t_set_t ntru_pks, trint9_t msg_type_id,
    bundle_transactions_t *const bundle, trit_t *const msg_id);

/**
 * Writes MAM packet into a bundle
 *
 * @param api - The API [in,out]
 * @param msg_id - The msg_id
 * @param payload - payload to write into the packet [in]
 * @param payload size - The payload size [in]
 * @param is_last_packet - indicate whether or not this is the last packet [in]
 * @param bundle - The bundle that the packet will be written into [out]
 *
 * @return return code
 */
retcode_t mam_api_bundle_write_packet(
    mam_api_t *const api, trit_t *const msg_id, tryte_t const *const payload,
    size_t const payload_size, mam_msg_checksum_t checksum, bool is_last_packet,
    bundle_transactions_t *const bundle);

/**
 * Reads MAM's session key and potentially the first packet using NTRU secret
 * key
 *
 * @param api - The API
 * @param bundle - The bundle containing the MAM message
 * @param packet_payload - First packet payload [out] (will be allocated if
 * packet is present)
 *
 * @return return code
 */
retcode_t mam_api_bundle_read(mam_api_t *const api,
                              bundle_transactions_t const *const bundle,
                              tryte_t **const payload,
                              size_t *const payload_size,
                              bool *const is_last_packet);

/**
 * Gets the number of trits needed for an API serialization
 * @param api - The API [in]
 *
 * @return return the size
 */
size_t mam_api_serialized_size(mam_api_t const *const api);

/**
 * Serializes an API struct into a buffer
 * @param api - The API [in]
 * @param buffer - The buffer to serialize the api into [out]
 *
 * @return return void
 */
void mam_api_serialize(mam_api_t const *const api, trits_t *const buffer);

/**
 * Deserializes a buffer into API struct
 *
 * @param buffer - The buffer from where to deserialize [in]
 * @param api - The API [out]
 *
 * @return return code
 */
retcode_t mam_api_deserialize(trits_t *const buffer, mam_api_t *const api);

/**
 * Saves an API into a file
 *
 * @param api - The API [in]
 * @param filename - The file name where to serialize the API into [in]

 *
 * @return return code
 */
retcode_t mam_api_save(mam_api_t const *const api, char const *const filename);

/**
 * Loads an API into a file
 *
 * @param filename - The file name where the API is serialized [in]
 * @param api - The API [out]
 *
 * @return return code
 */
retcode_t mam_api_load(char const *const filename, mam_api_t *const api);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_API_API_H__
