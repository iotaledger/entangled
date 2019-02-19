/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_MAM_CHANNEL_H__
#define __MAM_V2_MAM_CHANNEL_H__

#include "common/trinary/add.h"
#include "mam/v2/mam/mam_endpoint_t_set.h"
#include "mam/v2/mss/mss.h"
#include "mam/v2/trits/trits.h"

#define MAM2_CHANNEL_ID_SIZE MAM2_MSS_MT_HASH_SIZE
#define MAM2_CHANNEL_MSG_ORD_SIZE 81

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mam_channel_s {
  trit_t id[MAM2_CHANNEL_ID_SIZE];
  trits_t name;
  trit_t msg_ord[MAM2_CHANNEL_MSG_ORD_SIZE];
  mss_t mss;
  mam_endpoint_t_set_t endpoints;
} mam_channel_t;

typedef struct mam_channel_t_set_entry_s mam_channel_t_set_entry_t;
typedef mam_channel_t_set_entry_t *mam_channel_t_set_t;

/**
 * Gets a channel's id
 *
 * @param channel The channel
 *
 * @return the channel's id
 */
trits_t mam_channel_id(mam_channel_t const *const channel);

/**
 * Gets a channel's name
 *
 * @param channel The channel
 *
 * @return the channel's name
 */
trits_t mam_channel_name(mam_channel_t const *const channel);

/**
 * Gets a channel's msg_ord
 *
 * @param channel The channel
 *
 * @return the channel's msg_ord
 */
trits_t mam_channel_msg_ord(mam_channel_t const *const channel);

/**
 * Allocates memory for internal objects, and generates MSS public key
 *
 * @param allocator A MAM allocator
 * @param prng A shared PRNG interface used to generate WOTS private keys
 * @param height MSS MT height
 * @param channel_name The channel name
 * @param channel The channel
 *
 * @return a status code
 */
retcode_t mam_channel_create(mam_prng_t const *const prng,
                             mss_mt_height_t const height,
                             trits_t const channel_name,
                             mam_channel_t *const channel);

/**
 * Deallocates memory for internal objects
 * Before destroying channel, make sure to destroy all associated endpoints
 *
 * @param allocator A MAM allocator
 * @param channel The channel
 */
void mam_channel_destroy(mam_channel_t *const channel);

retcode_t mam_channels_destroy(mam_channel_t_set_t *const channels);

size_t mam_channel_serialized_size(mam_channel_t const *const channel);

retcode_t mam_channel_serialize(mam_channel_t const *const channel,
                                trits_t *const buffer);

retcode_t mam_channel_deserialize(trits_t *const buffer, mam_prng_t *const prng,
                                  mam_channel_t *const channel);

size_t mam_channels_serialized_size(mam_channel_t_set_t const channels);

retcode_t mam_channels_serialize(mam_channel_t_set_t const channels,
                                 trits_t *const buffer);

retcode_t mam_channels_deserialize(trits_t *const buffer,
                                   mam_prng_t *const prng,
                                   mam_channel_t_set_t *const channels);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_MAM_CHANNEL_H__
