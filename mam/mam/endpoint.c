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

retcode_t mam_endpoint_create(mam_prng_t *const prng, mss_mt_height_t const height, trits_t const channel_name_size,
                              trits_t const channel_name, trits_t const name, mam_endpoint_t *const endpoint) {
  MAM_ASSERT(endpoint);
  MAM_ASSERT(height <= MAM_MSS_MAX_D);

  retcode_t ret = RC_OK;
  size_t name_size = trits_size(name);
  trits_t name_size_trits = trits_null();

  MAM_ASSERT(name_size % NUMBER_OF_TRITS_IN_A_TRYTE == 0);

  memset(endpoint, 0, sizeof(mam_endpoint_t));

  if (trits_is_null(endpoint->name_size = trits_alloc(MAM_TRITS_MAX_SIZEOF_SIZE_T))) {
    ret = RC_OOM;
    goto done;
  }
  name_size_trits = mam_endpoint_name_size(endpoint);
  trits_set_zero(name_size_trits);
  trits_encode_size_t(name_size / NUMBER_OF_TRITS_IN_A_TRYTE, &name_size_trits);

  if (trits_is_null(endpoint->name = trits_alloc(name_size))) {
    ret = RC_OOM;
    goto done;
  }
  trits_copy(name, endpoint->name);

  if ((ret = mam_mss_create(&endpoint->mss, height)) != RC_OK) {
    goto done;
  }

  mam_mss_init(&endpoint->mss, prng, height, channel_name_size, channel_name, mam_endpoint_name_size(endpoint),
               mam_endpoint_name(endpoint));

  mam_mss_gen(&endpoint->mss);

done:

  if (ret != RC_OK) {
    trits_free(endpoint->name_size);
    trits_free(endpoint->name);
  }

  return ret;
}

void mam_endpoint_destroy(mam_endpoint_t *const endpoint) {
  MAM_ASSERT(endpoint);
  trits_free(endpoint->name_size);
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
  return pb3_sizeof_ntrytes(MAM_TRITS_MAX_SIZEOF_SIZE_T / NUMBER_OF_TRITS_IN_A_TRYTE) +             // name size
         pb3_sizeof_ntrytes(trits_size(endpoint->name) / NUMBER_OF_TRITS_IN_A_TRYTE) +              // name
         pb3_sizeof_ntrytes(mam_mss_serialized_size(&endpoint->mss) / NUMBER_OF_TRITS_IN_A_TRYTE);  // mss
}

void mam_endpoint_serialize(mam_endpoint_t const *const endpoint, trits_t *const buffer) {
  pb3_encode_ntrytes(mam_endpoint_name_size(endpoint), buffer);  // name size
  pb3_encode_ntrytes(mam_endpoint_name(endpoint), buffer);       // name
  mam_mss_serialize(&endpoint->mss, buffer);                     // mss
}

retcode_t mam_endpoint_deserialize(trits_t *const buffer, trits_t const channel_name_size, trits_t const channel_name,
                                   mam_prng_t *const prng, mam_endpoint_t *const endpoint) {
  MAM_ASSERT(!trits_is_null(channel_name));

  retcode_t ret = RC_OK;
  size_t size = 0;
  mss_mt_height_t height = 0;
  trits_t name_size_trits = trits_null();

  if (trits_is_null(endpoint->name_size = trits_alloc(MAM_TRITS_MAX_SIZEOF_SIZE_T))) {
    ret = RC_OOM;
    goto done;
  }
  name_size_trits = mam_endpoint_name_size(endpoint);

  if ((ret = pb3_decode_ntrytes(mam_endpoint_name_size(endpoint), buffer)) != RC_OK) {  // name size
    goto done;
  }

  if ((ret = pb3_decode_size_t(&size, &name_size_trits)) != RC_OK) {
    goto done;
  }

  if (trits_is_null(endpoint->name = trits_alloc(size * NUMBER_OF_TRITS_IN_A_TRYTE))) {
    ret = RC_OOM;
    goto done;
  }
  if ((ret = pb3_decode_ntrytes(mam_endpoint_name(endpoint), buffer)) != RC_OK) {  // name
    goto done;
  }

  MAM_TRITS_DEF(ts, MAM_MSS_SKN_SIZE);
  ts = MAM_TRITS_INIT(ts, MAM_MSS_SKN_SIZE);
  trits_set_zero(ts);

  trits_copy(trits_take(*buffer, 4), trits_take(ts, 4));
  height = trits_get6(ts);

  if ((ret = mam_mss_create(&endpoint->mss, height)) != RC_OK) {
    goto done;
  }

  mam_mss_init(&endpoint->mss, prng, height, channel_name_size, channel_name, mam_endpoint_name_size(endpoint),
               mam_endpoint_name(endpoint));

  if ((ret = mam_mss_deserialize(buffer, &endpoint->mss)) != RC_OK) {
    mam_mss_destroy(&endpoint->mss);
    goto done;
  }

done:

  if (ret != RC_OK) {
    trits_free(endpoint->name_size);
    trits_free(endpoint->name);
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

retcode_t mam_endpoints_deserialize(trits_t *const buffer, trits_t const channel_name_size, trits_t const channel_name,
                                    mam_prng_t *const prng, mam_endpoint_t_set_t *const endpoints) {
  retcode_t ret = RC_OK;
  mam_endpoint_t endpoint;
  size_t size = 0;

  if ((ret = pb3_decode_size_t(&size, buffer)) != RC_OK) {  // endpoints number
    return ret;
  }

  while (size--) {
    if ((ret = mam_endpoint_deserialize(buffer, channel_name_size, channel_name, prng,
                                        &endpoint)) != RC_OK) {  // endpoint
      return ret;
    }
    if ((ret = mam_endpoint_t_set_add(endpoints, &endpoint)) != RC_OK) {
      return ret;
    }
  }

  return ret;
}
