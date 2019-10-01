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

retcode_t mam_channel_create(mam_prng_t *const prng, mss_mt_height_t const height, trits_t const name,
                             mam_channel_t *const channel) {
  MAM_ASSERT(channel);
  MAM_ASSERT(height <= MAM_MSS_MAX_D);

  retcode_t ret = RC_OK;
  size_t name_size = trits_size(name);
  trits_t name_size_trits = trits_null();

  MAM_ASSERT(name_size % NUMBER_OF_TRITS_IN_A_TRYTE == 0);

  memset(channel, 0, sizeof(mam_channel_t));

  if (trits_is_null(channel->name_size = trits_alloc(MAM_TRITS_MAX_SIZEOF_SIZE_T))) {
    ret = RC_OOM;
    goto done;
  }
  name_size_trits = mam_channel_name_size(channel);
  trits_set_zero(name_size_trits);
  trits_encode_size_t(name_size / NUMBER_OF_TRITS_IN_A_TRYTE, &name_size_trits);

  if (trits_is_null(channel->name = trits_alloc(name_size))) {
    ret = RC_OOM;
    goto done;
  }
  trits_copy(name, channel->name);

  if ((ret = mam_mss_create(&channel->mss, height)) != RC_OK) {
    goto done;
  }

  mam_mss_init(&channel->mss, prng, height, mam_channel_name_size(channel), mam_channel_name(channel), trits_null(),
               trits_null());

  mam_mss_gen(&channel->mss);

done:

  if (ret != RC_OK) {
    trits_free(channel->name_size);
    trits_free(channel->name);
  }

  return ret;
}

void mam_channel_destroy(mam_channel_t *const channel) {
  MAM_ASSERT(channel);

  trits_free(channel->name_size);
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
  return pb3_sizeof_ntrytes(MAM_TRITS_MAX_SIZEOF_SIZE_T / NUMBER_OF_TRITS_IN_A_TRYTE) +             // name size
         pb3_sizeof_ntrytes(trits_size(channel->name) / NUMBER_OF_TRITS_IN_A_TRYTE) +               // name
         pb3_sizeof_ntrytes(MAM_CHANNEL_MSG_ORD_SIZE / NUMBER_OF_TRITS_IN_A_TRYTE) +                // msg_ord
         pb3_sizeof_ntrytes(mam_mss_serialized_size(&channel->mss) / NUMBER_OF_TRITS_IN_A_TRYTE) +  // mss
         pb3_sizeof_ntrytes(mam_endpoints_serialized_size(channel->endpoints) /
                            NUMBER_OF_TRITS_IN_A_TRYTE)  // endpoints
         + pb3_sizeof_longtrint();                       // endpoint_ord
}

void mam_channel_serialize(mam_channel_t const *const channel, trits_t *const buffer) {
  pb3_encode_ntrytes(mam_channel_name_size(channel), buffer);  // name size
  pb3_encode_ntrytes(mam_channel_name(channel), buffer);       // name
  pb3_encode_ntrytes(mam_channel_msg_ord(channel), buffer);    // msg_ord
  mam_mss_serialize(&channel->mss, buffer);                    // mss
  mam_endpoints_serialize(channel->endpoints, buffer);         // endpoints
  pb3_encode_longtrint(channel->endpoint_ord, buffer);         // endpoint_ord
}

retcode_t mam_channel_deserialize(trits_t *const buffer, mam_prng_t *const prng, mam_channel_t *const channel) {
  retcode_t ret = RC_OK;
  size_t size = 0;
  mss_mt_height_t height = 0;
  trits_t name_size_trits = trits_null();

  if (trits_is_null(channel->name_size = trits_alloc(MAM_TRITS_MAX_SIZEOF_SIZE_T))) {
    ret = RC_OOM;
    goto done;
  }
  name_size_trits = mam_channel_name_size(channel);

  if ((ret = pb3_decode_ntrytes(mam_channel_name_size(channel), buffer)) != RC_OK) {  // name size
    goto done;
  }

  if ((ret = pb3_decode_size_t(&size, &name_size_trits)) != RC_OK) {
    goto done;
  }

  if (trits_is_null(channel->name = trits_alloc(size * NUMBER_OF_TRITS_IN_A_TRYTE))) {
    ret = RC_OOM;
    goto done;
  }
  if ((ret = pb3_decode_ntrytes(mam_channel_name(channel), buffer)) != RC_OK) {  // name
    goto done;
  }

  if ((ret = pb3_decode_ntrytes(mam_channel_msg_ord(channel), buffer)) != RC_OK) {  // msg_ord
    goto done;
  }

  MAM_TRITS_DEF(ts, MAM_MSS_SKN_SIZE);
  ts = MAM_TRITS_INIT(ts, MAM_MSS_SKN_SIZE);
  trits_set_zero(ts);

  trits_copy(trits_take(*buffer, 4), trits_take(ts, 4));
  height = trits_get6(ts);

  if ((ret = mam_mss_create(&channel->mss, height)) != RC_OK) {
    goto done;
  }

  mam_mss_init(&channel->mss, prng, height, mam_channel_name_size(channel), mam_channel_name(channel), trits_null(),
               trits_null());

  if ((ret = mam_mss_deserialize(buffer, &channel->mss)) != RC_OK) {
    goto done;
  }

  channel->endpoints = NULL;
  mam_endpoints_deserialize(buffer, mam_channel_name_size(channel), mam_channel_name(channel), prng,
                            &channel->endpoints);                              // endpoints
  ERR_BIND_RETURN(pb3_decode_longtrint(&channel->endpoint_ord, buffer), ret);  // endpoint_ord

done:

  if (ret != RC_OK) {
    trits_free(channel->name_size);
    trits_free(channel->name);
  }

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
