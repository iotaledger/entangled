/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/mam/channel.h"
#include "common/defs.h"
#include "mam/mam/mam_channel_t_set.h"
#include "mam/pb3/pb3.h"

trits_t mam_channel_id(mam_channel_t const *const channel) {
  return trits_from_rep(MAM_CHANNEL_ID_SIZE, channel->mss.root);
}

trits_t mam_channel_name(mam_channel_t const *const channel) { return channel->name; }

trits_t mam_channel_msg_ord(mam_channel_t const *const channel) {
  return trits_from_rep(MAM_CHANNEL_MSG_ORD_SIZE, channel->msg_ord);
}

retcode_t mam_channel_create(mam_prng_t *const prng, mss_mt_height_t const height, trits_t const channel_name,
                             mam_channel_t *const channel) {
  MAM_ASSERT(channel);
  MAM_ASSERT(height <= MAM_MSS_MAX_D);

  retcode_t ret = RC_OK;

  if (trits_is_null(channel->name = trits_alloc(trits_size(channel_name)))) {
    return RC_OOM;
  }
  trits_copy(channel_name, channel->name);

  memset(channel->msg_ord, 0, MAM_CHANNEL_MSG_ORD_SIZE);

  if ((ret = mam_mss_create(&channel->mss, height)) != RC_OK) {
    trits_free(channel->name);
    channel->name = trits_null();
    return ret;
  }

  mam_mss_init(&channel->mss, prng, height, channel->name, trits_null());

  mam_mss_gen(&channel->mss);

  channel->endpoints = NULL;
  channel->endpoint_ord = 0;

  return ret;
}

size_t mam_channel_num_remaining_sks(mam_channel_t const *const channel) {
  return mam_mss_num_remaining_sks(&channel->mss);
}

void mam_channel_destroy(mam_channel_t *const channel) {
  MAM_ASSERT(channel);

  trits_free(channel->name);
  mam_mss_destroy(&channel->mss);
  mam_endpoints_destroy(&channel->endpoints);
}

retcode_t mam_channels_destroy(mam_channel_t_set_t *const channels) {
  mam_channel_t_set_entry_t *entry = NULL;
  mam_channel_t_set_entry_t *tmp = NULL;

  if (channels == NULL || *channels == NULL) {
    return RC_OK;
  }

  SET_ITER(*channels, entry, tmp) {
    mam_channel_destroy(&entry->value);
    mam_channel_t_set_remove_entry(channels, entry);
  }

  return RC_OK;
}

size_t mam_channel_serialized_size(mam_channel_t const *const channel) {
  return pb3_sizeof_size_t(trits_size(channel->name)) +                                             // name size
         pb3_sizeof_ntrytes(trits_size(channel->name) / NUMBER_OF_TRITS_IN_A_TRYTE) +               // name
         pb3_sizeof_ntrytes(MAM_CHANNEL_MSG_ORD_SIZE / NUMBER_OF_TRITS_IN_A_TRYTE) +                // msg_ord
         pb3_sizeof_ntrytes(mam_mss_serialized_size(&channel->mss) / NUMBER_OF_TRITS_IN_A_TRYTE) +  // mss
         pb3_sizeof_ntrytes(mam_endpoints_serialized_size(channel->endpoints) /
                            NUMBER_OF_TRITS_IN_A_TRYTE)  // endpoints
         + pb3_sizeof_longtrint();                       // endpoint_ord
}

void mam_channel_serialize(mam_channel_t const *const channel, trits_t *const buffer) {
  size_t mss_size = mam_mss_serialized_size(&channel->mss);

  pb3_encode_size_t(trits_size(channel->name), buffer);  // name size
  pb3_encode_ntrytes(channel->name, buffer);             // name
  pb3_encode_ntrytes(trits_from_rep(MAM_CHANNEL_MSG_ORD_SIZE, channel->msg_ord),
                     buffer);                                       // msg_ord
  mam_mss_serialize(&channel->mss, trits_take(*buffer, mss_size));  // mss
  trits_advance(buffer, mss_size);
  mam_endpoints_serialize(channel->endpoints, buffer);  // endpoints
  pb3_encode_longtrint(channel->endpoint_ord, buffer);  // endpoint_ord
}

retcode_t mam_channel_deserialize(trits_t *const buffer, mam_prng_t *const prng, mam_channel_t *const channel) {
  retcode_t ret = RC_OK;
  size_t size = 0;
  mss_mt_height_t height = 0;

  if ((ret = pb3_decode_size_t(&size, buffer)) != RC_OK) {  // name size
    return ret;
  }
  if (trits_is_null(channel->name = trits_alloc(size))) {
    return RC_OOM;
  }
  if ((ret = pb3_decode_ntrytes(channel->name, buffer)) != RC_OK) {  // name
    return ret;
  }

  if ((ret = pb3_decode_ntrytes(trits_from_rep(MAM_CHANNEL_MSG_ORD_SIZE, channel->msg_ord),
                                buffer)) != RC_OK) {  // msg_ord
    return ret;
  }

  MAM_TRITS_DEF(ts, MAM_MSS_SKN_SIZE);
  ts = MAM_TRITS_INIT(ts, MAM_MSS_SKN_SIZE);
  trits_set_zero(ts);

  trits_copy(trits_take(*buffer, 4), trits_take(ts, 4));
  height = trits_get6(ts);

  if ((ret = mam_mss_create(&channel->mss, height)) != RC_OK) {
    return ret;
  }

  mam_mss_init(&channel->mss, prng, height, channel->name, trits_null());

  if ((ret = mam_mss_deserialize(buffer, &channel->mss)) != RC_OK) {
    return ret;
  }

  channel->endpoints = NULL;
  mam_endpoints_deserialize(buffer, channel->name, prng,
                            &channel->endpoints);  // endpoints
  ERR_BIND_RETURN(pb3_decode_longtrint(&channel->endpoint_ord, buffer),
                  ret);  // endpoint_ord

  return ret;
}

size_t mam_channels_serialized_size(mam_channel_t_set_t const channels) {
  mam_channel_t_set_entry_t *entry = NULL;
  mam_channel_t_set_entry_t *tmp = NULL;
  size_t size = pb3_sizeof_size_t(mam_channel_t_set_size(channels));  // channels number

  SET_ITER(channels, entry, tmp) {  // channel
    size += mam_channel_serialized_size(&entry->value);
  }

  return size;
}

void mam_channels_serialize(mam_channel_t_set_t const channels, trits_t *const buffer) {
  mam_channel_t_set_entry_t *entry = NULL;
  mam_channel_t_set_entry_t *tmp = NULL;

  pb3_encode_size_t(mam_channel_t_set_size(channels),
                    buffer);  // channels number

  SET_ITER(channels, entry, tmp) { mam_channel_serialize(&entry->value, buffer); }
}

retcode_t mam_channels_deserialize(trits_t *const buffer, mam_prng_t *const prng, mam_channel_t_set_t *const channels) {
  retcode_t ret = RC_OK;
  mam_channel_t channel;
  size_t size = 0;

  if ((ret = pb3_decode_size_t(&size, buffer)) != RC_OK) {  // channels number
    return ret;
  }

  while (size--) {
    if ((ret = mam_channel_deserialize(buffer, prng, &channel)) != RC_OK) {  // channel
      return ret;
    }
    if ((ret = mam_channel_t_set_add(channels, &channel)) != RC_OK) {
      return ret;
    }
  }

  return ret;
}
