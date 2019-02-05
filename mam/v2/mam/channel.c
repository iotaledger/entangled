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
#include "mam/v2/mam/mam_channel_t_set.h"
#include "mam/v2/pb3/pb3.h"

trits_t mam_channel_id(mam_channel_t const *const channel) {
  return trits_from_rep(MAM2_CHANNEL_ID_SIZE, channel->id);
}

trits_t mam_channel_name(mam_channel_t const *const channel) {
  return channel->name;
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

  if ((ret = mam_mss_create(&channel->mss, prng, height, channel_name,
                            trits_null())) != RC_OK) {
    return ret;
  }

  mss_gen(&channel->mss, mam_channel_id(channel));

  channel->endpoints = NULL;

  return ret;
}

void mam_channel_destroy(mam_channel_t *const channel) {
  MAM2_ASSERT(channel);

  trits_free(channel->name);
  mam_mss_destroy(&channel->mss);
  mam_endpoints_destroy(&channel->endpoints);
}

size_t mam_channel_wrap_size() {
  // absorb tryte version + absorb external tryte channel_id[81]
  return pb3_sizeof_tryte() + 0;
}

void mam_channel_wrap(mam_spongos_t *const spongos, trits_t *const buffer,
                      tryte_t const version, trits_t const channel_id) {
  MAM2_ASSERT(mam_channel_wrap_size() <= trits_size(*buffer));
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(channel_id));

  // absorb tryte version
  pb3_wrap_absorb_tryte(spongos, buffer, version);
  // absorb external tryte channel_id[81]
  pb3_absorb_external_ntrytes(spongos, channel_id);
}

retcode_t mam_channel_unwrap(mam_spongos_t *const spongos,
                             trits_t *const buffer, tryte_t *const version,
                             trits_t channel_id) {
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(channel_id));

  retcode_t ret = RC_OK;

  // absorb tryte version
  if ((ret = pb3_unwrap_absorb_tryte(spongos, buffer, version)) != RC_OK) {
    return ret;
  }

  // absorb external tryte channel_id[81]
  pb3_absorb_external_ntrytes(spongos, channel_id);

  return ret;
}

size_t mam_channel_serialized_size(mam_channel_t const *const channel) {
  size_t mss_size = mss_stored_size(&channel->mss);
  size_t endpoints_size = mam_endpoints_serialized_size(channel->endpoints);

  return pb3_sizeof_ntrytes(MAM2_CHANNEL_ID_SIZE / 3) +       // id
         pb3_sizeof_size_t(trits_size(channel->name)) +       // name size
         pb3_sizeof_ntrytes(trits_size(channel->name) / 3) +  // name
         pb3_sizeof_size_t(mss_size) +                        // mss size
         pb3_sizeof_ntrytes(mss_size / 3) +                   // mss
         pb3_sizeof_ntrytes(endpoints_size / 3);              // endpoints
}

retcode_t mam_channel_serialize(mam_channel_t const *const channel,
                                trits_t *const buffer) {
  size_t mss_size = mss_stored_size(&channel->mss);

  pb3_encode_ntrytes(trits_from_rep(MAM2_CHANNEL_ID_SIZE, channel->id),
                     buffer);                              // id
  pb3_encode_size_t(trits_size(channel->name), buffer);    // name size
  pb3_encode_ntrytes(channel->name, buffer);               // name
  pb3_encode_size_t(mss_size, buffer);                     // mss size
  mss_save(&channel->mss, trits_take(*buffer, mss_size));  // mss
  trits_advance(buffer, mss_size);
  mam_endpoints_serialize(channel->endpoints, buffer);  // endpoints

  return RC_OK;
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

  if ((ret = mss_load(&channel->mss, buffer)) != RC_OK) {
    return ret;
  }

  mam_endpoints_deserialize(buffer, channel->name, prng,
                            &channel->endpoints);  // endpoints

  return ret;
}

size_t mam_channels_serialized_size(mam_channel_t_set_t const channels) {
  mam_channel_t_set_entry_t *entry = NULL;
  mam_channel_t_set_entry_t *tmp = NULL;
  size_t size =
      pb3_sizeof_size_t(mam_channel_t_set_size(channels));  // channels number

  HASH_ITER(hh, channels, entry, tmp) {
    size += mam_channel_serialized_size(&entry->value);  // channel
  }

  return size;
}

retcode_t mam_channels_serialize(mam_channel_t_set_t const channels,
                                 trits_t *const buffer) {
  retcode_t ret = RC_OK;
  mam_channel_t_set_entry_t *entry = NULL;
  mam_channel_t_set_entry_t *tmp = NULL;

  pb3_encode_size_t(mam_channel_t_set_size(channels),
                    buffer);  // channels number

  HASH_ITER(hh, channels, entry, tmp) {
    if ((ret = mam_channel_serialize(&entry->value, buffer)) !=
        RC_OK) {  // channel
      return ret;
    }
  }

  return ret;
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
