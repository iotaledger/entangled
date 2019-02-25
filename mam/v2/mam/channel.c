/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/mam/channel.h"
#include "common/defs.h"
#include "mam/v2/mam/mam_channel_t_set.h"
#include "mam/v2/pb3/pb3.h"

trits_t mam_channel_id(mam_channel_t const *const channel) {
  return trits_from_rep(MAM2_CHANNEL_ID_SIZE, channel->id);
}

trits_t mam_channel_name(mam_channel_t const *const channel) {
  return channel->name;
}

trits_t mam_channel_msg_ord(mam_channel_t const *const channel) {
  return trits_from_rep(MAM2_CHANNEL_MSG_ORD_SIZE, channel->msg_ord);
}

retcode_t mam_channel_create(mam_prng_t const *const prng,
                             mss_mt_height_t const height,
                             trits_t const channel_name,
                             mam_channel_t *const channel) {
  MAM2_ASSERT(channel);

  retcode_t ret = RC_OK;

  if (trits_is_null(channel->name = trits_alloc(trits_size(channel_name)))) {
    return RC_OOM;
  }
  trits_copy(channel_name, channel->name);

  memset(channel->msg_ord, 0, MAM2_CHANNEL_MSG_ORD_SIZE);

  if ((ret = mss_create(&channel->mss, height)) != RC_OK) {
    trits_free(channel->name);
    channel->name = trits_null();
    return ret;
  }

  mss_init(&channel->mss, prng, height, channel->name, trits_null());

  mss_gen(&channel->mss, mam_channel_id(channel));

  channel->endpoints = NULL;

  return ret;
}

void mam_channel_destroy(mam_channel_t *const channel) {
  MAM2_ASSERT(channel);

  trits_free(channel->name);
  mss_destroy(&channel->mss);
  mam_endpoints_destroy(&channel->endpoints);
}

retcode_t mam_channels_destroy(mam_channel_t_set_t *const channels) {
  mam_channel_t_set_entry_t *entry = NULL;
  mam_channel_t_set_entry_t *tmp = NULL;

  if (channels == NULL || *channels == NULL) {
    return RC_OK;
  }

  HASH_ITER(hh, *channels, entry, tmp) { mam_channel_destroy(&entry->value); }
  mam_channel_t_set_free(channels);

  return RC_OK;
}

size_t mam_channel_serialized_size(mam_channel_t const *const channel) {
  size_t mss_size = mss_serialized_size(&channel->mss);
  size_t endpoints_size = mam_endpoints_serialized_size(channel->endpoints);

  return pb3_sizeof_ntrytes(MAM2_CHANNEL_ID_SIZE /
                            NUMBER_OF_TRITS_IN_A_TRYTE) +  // id
         pb3_sizeof_size_t(trits_size(channel->name)) +    // name size
         pb3_sizeof_ntrytes(trits_size(channel->name) /
                            NUMBER_OF_TRITS_IN_A_TRYTE) +  // name
         pb3_sizeof_ntrytes(MAM2_CHANNEL_MSG_ORD_SIZE /
                            NUMBER_OF_TRITS_IN_A_TRYTE) +  // msg_ord
         pb3_sizeof_size_t(mss_size) +                     // mss size
         pb3_sizeof_ntrytes(mss_size / NUMBER_OF_TRITS_IN_A_TRYTE) +  // mss
         pb3_sizeof_ntrytes(endpoints_size /
                            NUMBER_OF_TRITS_IN_A_TRYTE);  // endpoints
}

void mam_channel_serialize(mam_channel_t const *const channel,
                           trits_t *const buffer) {
  size_t mss_size = mss_serialized_size(&channel->mss);

  pb3_encode_ntrytes(trits_from_rep(MAM2_CHANNEL_ID_SIZE, channel->id),
                     buffer);                            // id
  pb3_encode_size_t(trits_size(channel->name), buffer);  // name size
  pb3_encode_ntrytes(channel->name, buffer);             // name
  pb3_encode_ntrytes(
      trits_from_rep(MAM2_CHANNEL_MSG_ORD_SIZE, channel->msg_ord),
      buffer);                                                  // msg_ord
  pb3_encode_size_t(mss_size, buffer);                          // mss size
  mss_serialize(&channel->mss, trits_take(*buffer, mss_size));  // mss
  trits_advance(buffer, mss_size);
  mam_endpoints_serialize(channel->endpoints, buffer);  // endpoints
}

retcode_t mam_channel_deserialize(trits_t *const buffer, mam_prng_t *const prng,
                                  mam_channel_t *const channel) {
  retcode_t ret = RC_OK;
  size_t size = 0;
  mss_mt_height_t height = 0;

  if ((ret = pb3_decode_ntrytes(
           trits_from_rep(MAM2_CHANNEL_ID_SIZE, channel->id), buffer)) !=
      RC_OK) {  // id
    return ret;
  }

  if ((ret = pb3_decode_size_t(&size, buffer)) != RC_OK) {  // name size
    return ret;
  }
  if (trits_is_null(channel->name = trits_alloc(size))) {
    return RC_OOM;
  }
  if ((ret = pb3_decode_ntrytes(channel->name, buffer)) != RC_OK) {  // name
    return ret;
  }

  if ((ret = pb3_decode_ntrytes(
           trits_from_rep(MAM2_CHANNEL_MSG_ORD_SIZE, channel->msg_ord),
           buffer)) != RC_OK) {  // msg_ord
    return ret;
  }

  if ((ret = pb3_decode_size_t(&size, buffer)) != RC_OK) {  // mss size
    return ret;
  }

  MAM2_TRITS_DEF0(ts, MAM2_MSS_SKN_SIZE);
  ts = MAM2_TRITS_INIT(ts, MAM2_MSS_SKN_SIZE);
  trits_set_zero(ts);

  trits_copy(trits_take(*buffer, 4), trits_take(ts, 4));
  height = trits_get6(ts);

  if ((ret = mss_create(&channel->mss, height)) != RC_OK) {
    return ret;
  }

  mss_init(&channel->mss, prng, height, channel->name, trits_null());

  if ((ret = mss_deserialize(buffer, &channel->mss)) != RC_OK) {
    return ret;
  }

  channel->endpoints = NULL;
  mam_endpoints_deserialize(buffer, channel->name, prng,
                            &channel->endpoints);  // endpoints

  return ret;
}

size_t mam_channels_serialized_size(mam_channel_t_set_t const channels) {
  mam_channel_t_set_entry_t *entry = NULL;
  mam_channel_t_set_entry_t *tmp = NULL;
  size_t size =
      pb3_sizeof_size_t(mam_channel_t_set_size(channels));  // channels number

  HASH_ITER(hh, channels, entry, tmp) {  // channel
    size += mam_channel_serialized_size(&entry->value);
  }

  return size;
}

void mam_channels_serialize(mam_channel_t_set_t const channels,
                            trits_t *const buffer) {
  mam_channel_t_set_entry_t *entry = NULL;
  mam_channel_t_set_entry_t *tmp = NULL;

  pb3_encode_size_t(mam_channel_t_set_size(channels),
                    buffer);  // channels number

  HASH_ITER(hh, channels, entry, tmp) {
    mam_channel_serialize(&entry->value, buffer);
  }
}

retcode_t mam_channels_deserialize(trits_t *const buffer,
                                   mam_prng_t *const prng,
                                   mam_channel_t_set_t *const channels) {
  retcode_t ret = RC_OK;
  mam_channel_t channel;
  size_t size = 0;

  if ((ret = pb3_decode_size_t(&size, buffer)) != RC_OK) {  // channels number
    return ret;
  }

  while (size--) {
    if ((ret = mam_channel_deserialize(buffer, prng, &channel)) !=
        RC_OK) {  // channel
      return ret;
    }
    if ((ret = mam_channel_t_set_add(channels, &channel)) != RC_OK) {
      return ret;
    }
  }

  return ret;
}
