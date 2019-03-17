/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/mam/endpoint.h"
#include "common/defs.h"
#include "mam/mam/mam_endpoint_t_set.h"
#include "mam/pb3/pb3.h"

trits_t mam_endpoint_id(mam_endpoint_t const *const endpoint) {
  return trits_from_rep(MAM_ENDPOINT_ID_SIZE, endpoint->mss.root);
}

trits_t mam_endpoint_channel_name(mam_endpoint_t const *const endpoint) { return endpoint->mss.nonce1; }

trits_t mam_endpoint_name(mam_endpoint_t const *const endpoint) { return endpoint->name; }

retcode_t mam_endpoint_create(mam_prng_t *const prng, mss_mt_height_t const height, trits_t const channel_name,
                              trits_t const endpoint_name, mam_endpoint_t *const endpoint) {
  MAM_ASSERT(endpoint);
  MAM_ASSERT(height <= MAM_MSS_MAX_D);

  retcode_t ret = RC_OK;

  if (trits_is_null(endpoint->name = trits_alloc(trits_size(endpoint_name)))) {
    return RC_OOM;
  }
  trits_copy(endpoint_name, endpoint->name);

  if ((ret = mam_mss_create(&endpoint->mss, height)) != RC_OK) {
    trits_free(endpoint->name);
    endpoint->name = trits_null();
    return ret;
  }

  mam_mss_init(&endpoint->mss, prng, height, channel_name, endpoint->name);

  mam_mss_gen(&endpoint->mss);

  return ret;
}

size_t mam_endpoint_num_remaining_sks(mam_endpoint_t const *const endpoint) {
  return mam_mss_num_remaining_sks(&endpoint->mss);
}

void mam_endpoint_destroy(mam_endpoint_t *const endpoint) {
  MAM_ASSERT(endpoint);
  trits_free(endpoint->name);
  mam_mss_destroy(&endpoint->mss);
}

retcode_t mam_endpoints_destroy(mam_endpoint_t_set_t *const endpoints) {
  mam_endpoint_t_set_entry_t *entry = NULL;
  mam_endpoint_t_set_entry_t *tmp = NULL;

  if (endpoints == NULL || *endpoints == NULL) {
    return RC_OK;
  }

  SET_ITER(*endpoints, entry, tmp) {
    mam_endpoint_destroy(&entry->value);
    mam_endpoint_t_set_remove_entry(endpoints, entry);
  }

  return RC_OK;
}

size_t mam_endpoint_serialized_size(mam_endpoint_t const *const endpoint) {
  return pb3_sizeof_size_t(trits_size(endpoint->name)) +                                            // name size
         pb3_sizeof_ntrytes(trits_size(endpoint->name) / NUMBER_OF_TRITS_IN_A_TRYTE) +              // name
         pb3_sizeof_ntrytes(mam_mss_serialized_size(&endpoint->mss) / NUMBER_OF_TRITS_IN_A_TRYTE);  // mss
}

void mam_endpoint_serialize(mam_endpoint_t const *const endpoint, trits_t *const buffer) {
  size_t mss_size = mam_mss_serialized_size(&endpoint->mss);

  pb3_encode_size_t(trits_size(endpoint->name), buffer);             // name size
  pb3_encode_ntrytes(endpoint->name, buffer);                        // name
  mam_mss_serialize(&endpoint->mss, trits_take(*buffer, mss_size));  // mss
  trits_advance(buffer, mss_size);
}

retcode_t mam_endpoint_deserialize(trits_t *const buffer, trits_t const channel_name, mam_prng_t *const prng,
                                   mam_endpoint_t *const endpoint) {
  MAM_ASSERT(!trits_is_null(channel_name));

  retcode_t ret = RC_OK;
  size_t size = 0;
  mss_mt_height_t height = 0;

  if ((ret = pb3_decode_size_t(&size, buffer)) != RC_OK) {  // name size
    return ret;
  }
  if (trits_is_null(endpoint->name = trits_alloc(size))) {
    return RC_OOM;
  }
  if ((ret = pb3_decode_ntrytes(endpoint->name, buffer)) != RC_OK) {  // name
    return ret;
  }

  MAM_TRITS_DEF0(ts, MAM_MSS_SKN_SIZE);
  ts = MAM_TRITS_INIT(ts, MAM_MSS_SKN_SIZE);
  trits_set_zero(ts);

  trits_copy(trits_take(*buffer, 4), trits_take(ts, 4));
  height = trits_get6(ts);

  if ((ret = mam_mss_create(&endpoint->mss, height)) != RC_OK) {
    return ret;
  }

  mam_mss_init(&endpoint->mss, prng, height, channel_name, endpoint->name);

  if ((ret = mam_mss_deserialize(buffer, &endpoint->mss)) != RC_OK) {
    mam_mss_destroy(&endpoint->mss);
    return ret;
  }

  return ret;
}

size_t mam_endpoints_serialized_size(mam_endpoint_t_set_t const endpoints) {
  mam_endpoint_t_set_entry_t *entry = NULL;
  mam_endpoint_t_set_entry_t *tmp = NULL;
  size_t size = pb3_sizeof_size_t(mam_endpoint_t_set_size(endpoints));  // endpoints number

  SET_ITER(endpoints, entry, tmp) {  // endpoint
    size += mam_endpoint_serialized_size(&entry->value);
  }

  return size;
}

void mam_endpoints_serialize(mam_endpoint_t_set_t const endpoints, trits_t *const buffer) {
  mam_endpoint_t_set_entry_t *entry = NULL;
  mam_endpoint_t_set_entry_t *tmp = NULL;

  pb3_encode_size_t(mam_endpoint_t_set_size(endpoints),
                    buffer);  // endpoints number

  SET_ITER(endpoints, entry, tmp) { mam_endpoint_serialize(&entry->value, buffer); }
}

retcode_t mam_endpoints_deserialize(trits_t *const buffer, trits_t const channel_name, mam_prng_t *const prng,
                                    mam_endpoint_t_set_t *const endpoints) {
  retcode_t ret = RC_OK;
  mam_endpoint_t endpoint;
  size_t size = 0;

  if ((ret = pb3_decode_size_t(&size, buffer)) != RC_OK) {  // endpoints number
    return ret;
  }

  while (size--) {
    if ((ret = mam_endpoint_deserialize(buffer, channel_name, prng,
                                        &endpoint)) != RC_OK) {  // endpoint
      return ret;
    }
    if ((ret = mam_endpoint_t_set_add(endpoints, &endpoint)) != RC_OK) {
      return ret;
    }
  }

  return ret;
}
