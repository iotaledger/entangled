/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup mam
 *
 * @{
 *
 * @file
 * @brief
 *
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
#include "mam/prng/prng.h"
#include "mam/psk/psk.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mam_api_s {
  mam_prng_t prng;
  mam_ntru_sk_t_set_t ntru_sks;
  mam_ntru_pk_t_set_t ntru_pks;
  mam_psk_t_set_t psks;
  mam_channel_t_set_t channels;
  trint18_t channel_ord;
  trit_t_to_mam_msg_write_context_t_map_t write_ctxs;
  trit_t_to_mam_msg_read_context_t_map_t read_ctxs;
  mam_pk_t_set_t trusted_channel_pks;
  mam_pk_t_set_t trusted_endpoint_pks;
} mam_api_t;

/**
 * Initialize an API
 *
 * @param api - The API [out]
 * @param mam_seed - The seed for PRNG initialization [in]
 *
 * @return return code
 */
MAM_EXPORT retcode_t mam_api_init(mam_api_t *const api, tryte_t const *const mam_seed);

/**
 * Destroys an API
 *
 * @param api - The API [in,out]
 *
 * @return return code
 */
MAM_EXPORT retcode_t mam_api_destroy(mam_api_t *const api);

/**
 * Add a trusted channel id into the api's trusted channels set
 *
 * @param api - The API [in,out]
 * @param pk - A new public key [in]
 *
 * @return return code
 */
MAM_EXPORT retcode_t mam_api_add_trusted_channel_pk(mam_api_t *const api, tryte_t const *const pk);

/**
 * Add a trusted endpoint id into the api's trusted endpoints set
 *
 * @param api - The API [in,out]
 * @param pk - A new public key [in]
 *
 * @return return code
 */
MAM_EXPORT retcode_t mam_api_add_trusted_endpoint_pk(mam_api_t *const api, tryte_t const *const pk);

/**
 * Add a NTRU secret key to api's NTRU sks set
 *
 * @param api - The API [in,out]
 * @param ntru_sk - A new ntru public key (allows for both enc/dec) [in]
 *
 * @return return code
 */
MAM_EXPORT retcode_t mam_api_add_ntru_sk(mam_api_t *const api, mam_ntru_sk_t const *const ntru_sk);

/**
 * Add a NTRU public key to api's NTRU pks set
 *
 * @param api - The API [in,out]
 * @param ntru_pk - A new ntru public key (allows for encryption only) [in]
 *
 * @return return code
 */
MAM_EXPORT retcode_t mam_api_add_ntru_pk(mam_api_t *const api, mam_ntru_pk_t const *const ntru_pk);

/**
 * Add a pre shared key to api's psks set
 *
 * @param api - The API [in,out]
 * @param psk - A new psk [in]
 *
 * @return return code
 */
MAM_EXPORT retcode_t mam_api_add_psk(mam_api_t *const api, mam_psk_t const *const psk);

/**
 * Creates and adds a channel to the API
 *
 * @param api - The API [in, out]
 * @param height - The channel's MSS height [in]
 * @param channel_id - The channel id [out]
 *
 * @return return code
 */
MAM_EXPORT retcode_t mam_api_channel_create(mam_api_t *const api, size_t const height, tryte_t *const channel_id);

/**
 * Returns the number of remaining secret keys of a channel
 *
 * @param[in] api - The API
 * @param[in] channel_id - The channel id
 *
 * @return the number of remaining secret keys of the channel
 */
MAM_EXPORT size_t mam_api_channel_remaining_sks(mam_api_t *const api, tryte_t const *const channel_id);

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
MAM_EXPORT retcode_t mam_api_endpoint_create(mam_api_t *const api, size_t const height, tryte_t const *const channel_id,
                                             tryte_t *const endpoint_id);

/**
 * Returns the number of remaining secret keys of an endpoint
 *
 * @param[in] api - The API
 * @param[in] channel_id - The associated channel id
 * @param[in] endpoint_id - The endpoint id
 *
 * @return the number of remaining secret keys of the endpoint
 */
MAM_EXPORT size_t mam_api_endpoint_remaining_sks(mam_api_t *const api, tryte_t const *const channel_id,
                                                 tryte_t const *const endpoint_id);

/**
 * Creates a MAM tag that can be used in IOTA transactions
 *
 * @param tag - The tag [out]
 * @param msg_id - The message ID [in]
 * @param ord - The packet ord [in]
 */
MAM_EXPORT void mam_api_write_tag(trit_t *const tag, trit_t const *const msg_id, trint18_t const ord);

/**
 * Writes MAM header on a channel(keyloads (session keys) + potential packet)
 * into a bundle
 *
 * @param api - The API [in,out]
 * @param ch_id - A known channel ID [in]
 * @param psks - pre shared keys used for encrypting the session keys [in]
 * @param ntru_pks - ntru public keys used for encrypting the session keys [in]
 * @param bundle - The bundle that the packet will be written into [out]
 * @param msg_id - The msg_id (hashed channel_name and message index within the
 *  channel) embedded into transaction's tag (together with packet index to
 * allow Tangle lookup) [out]
 *
 * @return return code
 */
MAM_EXPORT retcode_t mam_api_bundle_write_header_on_channel(mam_api_t *const api, tryte_t const *const ch_id,
                                                            mam_psk_t_set_t psks, mam_ntru_pk_t_set_t ntru_pks,
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
 * @param bundle - The bundle that the packet will be written into [out]
 * @param msg_id - The msg_id (hashed channel_name and message index within the
 *  channel) embedded into transaction's tag (together with packet index to
 * allow Tangle lookup) [out]
 *
 * @return return code
 */
MAM_EXPORT retcode_t mam_api_bundle_write_header_on_endpoint(mam_api_t *const api, tryte_t const *const ch_id,
                                                             tryte_t const *const ep_id, mam_psk_t_set_t psks,
                                                             mam_ntru_pk_t_set_t ntru_pks,
                                                             bundle_transactions_t *const bundle, trit_t *const msg_id);

/**
 * Writes an announcement of a channel (keyloads (session keys) + potential packet) into a bundle
 *
 * @param api - The API [in,out]
 * @param ch_id - A known channel ID [in]
 * @param ch1_id - The new channel ID [in]
 * @param psks - pre shared keys used for encrypting the session keys [in]
 * @param ntru_pks - ntru public keys used for encrypting the session keys [in]
 * @param bundle - The bundle that the packet will be written into [out]
 * @param msg_id - The msg_id (hashed channel_name and message index within the
 *  channel) embedded into transaction's tag (together with packet index to
 * allow Tangle lookup) [out]
 *
 * @return return code
 */
MAM_EXPORT retcode_t mam_api_bundle_announce_channel(mam_api_t *const api, tryte_t const *const ch_id,
                                                     tryte_t const *const ch1_id, mam_psk_t_set_t psks,
                                                     mam_ntru_pk_t_set_t ntru_pks, bundle_transactions_t *const bundle,
                                                     trit_t *const msg_id);

/**
 * Writes an announcement of a endpoint (keyloads (session keys) + potential packet) into a bundle
 *
 * @param api - The API [in,out]
 * @param ch_id - A known channel ID [in]
 * @param ep1_id - The new endpoint ID [in]
 * @param psks - pre shared keys used for encrypting the session keys [in]
 * @param ntru_pks - ntru public keys used for encrypting the session keys [in]
 * @param bundle - The bundle that the packet will be written into [out]
 * @param msg_id - The msg_id (hashed channel_name and message index within the
 *  channel) embedded into transaction's tag (together with packet index to
 * allow Tangle lookup) [out]
 *
 * @return return code
 */
MAM_EXPORT retcode_t mam_api_bundle_announce_endpoint(mam_api_t *const api, tryte_t const *const ch_id,
                                                      tryte_t const *const ep1_id, mam_psk_t_set_t psks,
                                                      mam_ntru_pk_t_set_t ntru_pks, bundle_transactions_t *const bundle,
                                                      trit_t *const msg_id);
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
MAM_EXPORT retcode_t mam_api_bundle_write_packet(mam_api_t *const api, trit_t const *const msg_id,
                                                 tryte_t const *const payload, size_t const payload_size,
                                                 mam_msg_checksum_t checksum, bool is_last_packet,
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
MAM_EXPORT retcode_t mam_api_bundle_read(mam_api_t *const api, bundle_transactions_t const *const bundle,
                                         tryte_t **const payload, size_t *const payload_size,
                                         bool *const is_last_packet);

/**
 * Gets the number of trits needed for an API serialization
 * @param api - The API [in]
 *
 * @return return the size
 */
MAM_EXPORT size_t mam_api_serialized_size(mam_api_t const *const api);

/**
 * Serializes an API struct into a buffer
 * @param api - The API [in]
 * @param buffer - The buffer to serialize the api into [out]
 * @param encr_key_trytes - The encryption key [in] (optional - can set null)
 * @param encr_key_trytes_size - The encryption key size[in]
 *
 * @return return void
 */
MAM_EXPORT void mam_api_serialize(mam_api_t const *const api, trit_t *const buffer,
                                  tryte_t const *const encr_key_trytes, size_t encr_key_trytes_size);

/**
 * Deserializes a buffer into API struct
 *
 * @param buffer - The buffer from where to deserialize [in]
 * @param buffer_size - The size of the buffer [in]
 * @param api - The API [out]
 * @param decr_key_trytes - The decryption key (encryption as well) [in] (optional - can set null)
 * @param decr_key_trytes_size - The encryption key size[in]
 *
 * @return return code
 */
MAM_EXPORT retcode_t mam_api_deserialize(trit_t const *const buffer, size_t const buffer_size, mam_api_t *const api,
                                         tryte_t const *const decr_key_trytes, size_t decr_key_trytes_size);

/**
 * Saves an API into a file
 *
 * @param api - The API [in]
 * @param filename - The file name where to serialize the API into [in]
 * @param encr_key_trytes - The encryption key [in] (optional - can set null)
 * @param encr_key_trytes_size - The encryption key size[in]
 *
 * @return return code
 */
MAM_EXPORT retcode_t mam_api_save(mam_api_t const *const api, char const *const filename,
                                  tryte_t const *const encr_key_trytes, size_t encr_key_trytes_size);

/**
 * Loads an API from a file
 *
 * @param filename - The file name where the API is serialized [in]
 * @param api - The API [out]
 * @param decr_key_trytes - The decryption key (encryption as well) [in] (optional - can set null)
 * @param decr_key_trytes_size - The encryption key size[in]
 *
 * @return return code
 */
MAM_EXPORT retcode_t mam_api_load(char const *const filename, mam_api_t *const api,
                                  tryte_t const *const decr_key_trytes, size_t decr_key_trytes_size);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_API_API_H__

/** @} */
