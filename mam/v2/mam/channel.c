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
#include "mam/v2/pb3/pb3.h"

trits_t mam_channel_id(mam_channel_t const *const channel) {
  return trits_from_rep(MAM2_CHANNEL_ID_SIZE, channel->id);
}

trits_t mam_channel_name(mam_channel_t const *const channel) {
  return channel->mss.nonce1;
}

retcode_t mam_channel_create(mam_ialloc_t const *const allocator,
                             prng_t const *const prng,
                             mss_mt_height_t const height,
                             trits_t const channel_name,
                             mam_channel_t *const channel) {
  MAM2_ASSERT(allocator);
  MAM2_ASSERT(channel);

  retcode_t ret = RC_OK;

  if ((ret = mam_mss_create(allocator, &channel->mss, prng, height,
                            channel_name, trits_null())) != RC_OK) {
    return ret;
  }

  mss_gen(&channel->mss, mam_channel_id(channel));

  return ret;
}

void mam_channel_destroy(mam_ialloc_t const *const allocator,
                         mam_channel_t *const channel) {
  MAM2_ASSERT(allocator);
  MAM2_ASSERT(channel);

  mam_mss_destroy(allocator, &channel->mss);
}

size_t mam_channel_wrap_size() {
  // absorb tryte version + absorb external tryte channel_id[81]
  return pb3_sizeof_tryte() + 0;
}

void mam_channel_wrap(spongos_t *const spongos, trits_t *const buffer,
                      tryte_t const version, trits_t const channel_id) {
  MAM2_ASSERT(mam_channel_wrap_size() <= trits_size(*buffer));
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(channel_id));

  // absorb tryte version
  pb3_wrap_absorb_tryte(spongos, buffer, version);
  // absorb external tryte channel_id[81]
  pb3_absorb_external_ntrytes(spongos, channel_id);
}

retcode_t mam_channel_unwrap(spongos_t *const spongos, trits_t *const buffer,
                             tryte_t *const version, trits_t channel_id) {
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
