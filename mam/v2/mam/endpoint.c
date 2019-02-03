/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/mam/endpoint.h"

trits_t mam_endpoint_id(mam_endpoint_t const *const endpoint) {
  return trits_from_rep(MAM2_ENDPOINT_ID_SIZE, endpoint->id);
}

trits_t mam_endpoint_channel_name(mam_endpoint_t const *const endpoint) {
  return endpoint->mss.nonce1;
}

trits_t mam_endpoint_name(mam_endpoint_t const *const endpoint) {
  return endpoint->mss.nonce2;
}

retcode_t mam_endpoint_create(mam_ialloc_t const *const allocator,
                              mam_prng_t const *const prng,
                              mss_mt_height_t const height,
                              trits_t const channel_name,
                              trits_t const endpoint_name,
                              mam_endpoint_t *const endpoint) {
  MAM2_ASSERT(allocator);
  MAM2_ASSERT(endpoint);

  retcode_t ret = RC_OK;

  if ((ret = mam_mss_create(allocator, &endpoint->mss, prng, height,
                            channel_name, endpoint_name)) != RC_OK) {
    return ret;
  }

  mss_gen(&endpoint->mss, mam_endpoint_id(endpoint));

  return ret;
}

void mam_endpoint_destroy(mam_ialloc_t const *const allocator,
                          mam_endpoint_t *const endpoint) {
  MAM2_ASSERT(allocator);
  MAM2_ASSERT(endpoint);

  mam_mss_destroy(allocator, &endpoint->mss);
}
