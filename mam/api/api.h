/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by [ITSec Lab]
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
 * @brief Initializes an API
 *
 * @param[out]  api   The API
 * @param[in]   seed  The seed
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_init(mam_api_t *const api, tryte_t const *const seed);

/**
 * @brief Destroys an API
 *
 * @param[in,out] api The API
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_destroy(mam_api_t *const api);

/**
 * @brief Adds a trusted channel pk into the api's trusted channels set
 *
 * @param[in,out] api The API
 * @param[in]     pk  A channel public key
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_add_trusted_channel_pk(mam_api_t *const api, tryte_t const *const pk);

/**
 * @brief Adds a trusted endpoint pk into the api's trusted endpoints set
 *
 * @param[in,out] api The API
 * @param[in]     pk  An endpoint public key
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_add_trusted_endpoint_pk(mam_api_t *const api, tryte_t const *const pk);

/**
 * @brief Adds a NTRU secret key to api's NTRU sks set
 *
 * @param[in,out] api     The API
 * @param[in]     ntru_sk A NTRU secret key
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_add_ntru_sk(mam_api_t *const api, mam_ntru_sk_t const *const ntru_sk);

/**
 * @brief Adds a NTRU public key to api's NTRU pks set
 *
 * @param[in,out] api     The API
 * @param[in]     ntru_pk A NTRU public key
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_add_ntru_pk(mam_api_t *const api, mam_ntru_pk_t const *const ntru_pk);

/**
 * @brief Adds a pre-shared key to api's psks set
 *
 * @param[in,out] api The API
 * @param[in]     psk A PSK
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_add_psk(mam_api_t *const api, mam_psk_t const *const psk);

/**
 * @brief Creates and adds a channel to the API
 *
 * @param[in, out]  api         The API
 * @param[in]       height      The channel's MSS height
 * @param[out]      channel_id  The channel id
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_channel_create(mam_api_t *const api, size_t const height, tryte_t *const channel_id);

/**
 * @brief Returns the number of remaining secret keys of a channel
 *
 * @param[in] api         The API
 * @param[in] channel_id  The channel id
 *
 * @return the number of remaining secret keys of the channel
 */
MAM_EXPORT size_t mam_api_channel_remaining_sks(mam_api_t const *const api, tryte_t const *const channel_id);

/**
 * @brief Creates and adds an endpoint to the API
 *
 * @param[in, out]  api         The API
 * @param[in]       height      The endpoint's MSS height
 * @param[in]       channel_id  The parent channel id
 * @param[out]      endpoint_id The endpoint id
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_endpoint_create(mam_api_t *const api, size_t const height, tryte_t const *const channel_id,
                                             tryte_t *const endpoint_id);

/**
 * @brief Returns the number of remaining secret keys of an endpoint
 *
 * @param[in] api         The API
 * @param[in] channel_id  The parent channel id
 * @param[in] endpoint_id The endpoint id
 *
 * @return the number of remaining secret keys of the endpoint
 */
MAM_EXPORT size_t mam_api_endpoint_remaining_sks(mam_api_t const *const api, tryte_t const *const channel_id,
                                                 tryte_t const *const endpoint_id);

/**
 * @brief Creates a MAM tag that can be used in IOTA transactions
 *
 * @param[out]  tag     The tag
 * @param[in]   msg_id  The message ID
 * @param[in]   ord     The packet ord
 */
MAM_EXPORT void mam_api_write_tag(trit_t *const tag, trit_t const *const msg_id, trint18_t const ord);

/**
 * @brief Writes a MAM header through a channel into a bundle
 *
 * @param[in,out] api       The API
 * @param[in]     ch_id     The channel ID
 * @param[in]     psks      Pre-Shared Keys used for encrypting the session key
 * @param[in]     ntru_pks  NTRU public keys used for encrypting the session key
 * @param[out]    bundle    The bundle where the packet will be written
 * @param[out]    msg_id    The new message ID, unique identifier of the message in the channel
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_bundle_write_header_on_channel(mam_api_t *const api, tryte_t const *const ch_id,
                                                            mam_psk_t_set_t const psks,
                                                            mam_ntru_pk_t_set_t const ntru_pks,
                                                            bundle_transactions_t *const bundle, trit_t *const msg_id);

/**
 * @brief Writes a MAM header through an endpoint into a bundle
 *
 * @param[in,out] api       The API
 * @param[in]     ch_id     The parent channel ID
 * @param[in]     ep_id     The endpoint ID
 * @param[in]     psks      Pre-Shared Keys used for encrypting the session key
 * @param[in]     ntru_pks  NTRU public keys used for encrypting the session key
 * @param[out]    bundle    The bundle where the packet will be written
 * @param[out]    msg_id    The new message ID, unique identifier of the message in the channel
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_bundle_write_header_on_endpoint(mam_api_t *const api, tryte_t const *const ch_id,
                                                             tryte_t const *const ep_id, mam_psk_t_set_t const psks,
                                                             mam_ntru_pk_t_set_t const ntru_pks,
                                                             bundle_transactions_t *const bundle, trit_t *const msg_id);

/**
 * @brief Writes an announcement of a channel into a bundle
 *
 * @param[in,out] api       The API
 * @param[in]     ch_id     The channel ID
 * @param[in]     ch1_id    The new channel ID
 * @param[in]     psks      Pre-Shared Keys used for encrypting the session key
 * @param[in]     ntru_pks  NTRU public keys used for encrypting the session key
 * @param[out]    bundle    The bundle where the packet will be written
 * @param[out]    msg_id    The new message ID, unique identifier of the message in the channel
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_bundle_announce_channel(mam_api_t *const api, tryte_t const *const ch_id,
                                                     tryte_t const *const ch1_id, mam_psk_t_set_t const psks,
                                                     mam_ntru_pk_t_set_t const ntru_pks,
                                                     bundle_transactions_t *const bundle, trit_t *const msg_id);

/**
 * @brief Writes an announcement of a endpoint into a bundle
 *
 * @param[in,out] api       The API
 * @param[in]     ch_id     The channel ID
 * @param[in]     ep1_id    The new endpoint ID
 * @param[in]     psks      Pre-Shared Keys used for encrypting the session key
 * @param[in]     ntru_pks  NTRU public keys used for encrypting the session key
 * @param[out]    bundle    The bundle where the packet will be written
 * @param[out]    msg_id    The new message ID, unique identifier of the message in the channel
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_bundle_announce_endpoint(mam_api_t *const api, tryte_t const *const ch_id,
                                                      tryte_t const *const ep1_id, mam_psk_t_set_t const psks,
                                                      mam_ntru_pk_t_set_t const ntru_pks,
                                                      bundle_transactions_t *const bundle, trit_t *const msg_id);
/**
 * @brief Writes a MAM packet into a bundle
 *
 * @param[in,out] api             The API
 * @param[in]     msg_id          The message ID
 * @param[in]     payload         Payload to write into the packet
 * @param[in]     payload size    The payload size
 * @param[in]     is_last_packet  Indicates whether or not this is the last packet
 * @param[out]    bundle          The bundle where the packet will be written
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_bundle_write_packet(mam_api_t *const api, trit_t const *const msg_id,
                                                 tryte_t const *const payload, size_t const payload_size,
                                                 mam_msg_checksum_t checksum, bool is_last_packet,
                                                 bundle_transactions_t *const bundle);

/**
 * @brief Reads a MAM header and potentially a MAM packet from a bundle
 *
 * @param[in,out] api             The API
 * @param[in]     bundle          The bundle containing the MAM message
 * @param[out]    payload         Payload of the packet (allocated if present)
 * @param[out]    payload_size    Size of the payload (non-null if present)
 * @param[out]    is_last_packet  True if it was the last packet, false otherwise
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_bundle_read(mam_api_t *const api, bundle_transactions_t const *const bundle,
                                         tryte_t **const payload, size_t *const payload_size,
                                         bool *const is_last_packet);

/**
 * @brief Gets the number of trits needed to serialize an API
 *
 * @param[in] api The API
 *
 * @return return the size
 */
MAM_EXPORT size_t mam_api_serialized_size(mam_api_t const *const api);

/**
 * @brief Serializes an API into a buffer
 *
 * @param[in]   api                   The API
 * @param[out]  buffer                The buffer to serialize the api into
 * @param[in]   encr_key_trytes       The encryption key (optional - can be set to null)
 * @param[in]   encr_key_trytes_size  The encryption key size
 *
 * @return return void
 */
MAM_EXPORT void mam_api_serialize(mam_api_t const *const api, trit_t *const buffer,
                                  tryte_t const *const encr_key_trytes, size_t encr_key_trytes_size);

/**
 * @brief Deserializes a buffer into an API
 *
 * @param[in]   buffer                The buffer from where to deserialize
 * @param[in]   buffer_size           The size of the buffer
 * @param[out]  api                   The API
 * @param[in]   decr_key_trytes       The decryption key (optional - can be set to null)
 * @param[in]   decr_key_trytes_size  The decryption key size
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_deserialize(trit_t const *const buffer, size_t const buffer_size, mam_api_t *const api,
                                         tryte_t const *const decr_key_trytes, size_t decr_key_trytes_size);

/**
 * @brief Saves an API into a file
 *
 * @param[in] api                   The API
 * @param[in] filename              The file name where to serialize the API into
 * @param[in] encr_key_trytes       The encryption key (optional - can be set to null)
 * @param[in] encr_key_trytes_size  The encryption key size
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_save(mam_api_t const *const api, char const *const filename,
                                  tryte_t const *const encr_key_trytes, size_t encr_key_trytes_size);

/**
 * @brief Loads an API from a file
 *
 * @param[in]   filename              The file name where the API is serialized
 * @param[out]  api                   The API
 * @param[in]   decr_key_trytes       The decryption key (optional - can be set to null)
 * @param[in]   decr_key_trytes_size  The decryption key size
 *
 * @return a status code
 */
MAM_EXPORT retcode_t mam_api_load(char const *const filename, mam_api_t *const api,
                                  tryte_t const *const decr_key_trytes, size_t decr_key_trytes_size);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_API_API_H__

/** @} */
