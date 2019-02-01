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
#include "mam/v2/mam/mam_endpoint_t_set.h"
#include "mam/v2/pb3/pb3.h"

trits_t mam_endpoint_id(mam_endpoint_t const *const endpoint) {
  return trits_from_rep(MAM2_ENDPOINT_ID_SIZE, endpoint->id);
}

trits_t mam_endpoint_channel_name(mam_endpoint_t const *const endpoint) {
  return endpoint->mss.nonce1;
}

trits_t mam_endpoint_name(mam_endpoint_t const *const endpoint) {
  return endpoint->mss.nonce2;
}

retcode_t mam_endpoint_create(mam_prng_t const *const prng,
                              mss_mt_height_t const height,
                              trits_t const channel_name,
                              trits_t const endpoint_name,
                              mam_endpoint_t *const endpoint) {
  MAM2_ASSERT(endpoint);

  retcode_t ret = RC_OK;

  if (trits_is_null(endpoint->name = trits_alloc(trits_size(endpoint_name)))) {
    return RC_OOM;
  }
  trits_copy(endpoint_name, endpoint->name);

  if ((ret = mam_mss_create(allocator, &endpoint->mss, prng, height,
                            channel_name, endpoint_name)) != RC_OK) {
    return ret;
  }

  mss_gen(&endpoint->mss, mam_endpoint_id(endpoint));

  return ret;
}

void mam_endpoint_destroy(mam_endpoint_t *const endpoint) {
  MAM2_ASSERT(endpoint);

  trits_free(endpoint->name);
  mam_mss_destroy(allocator, &endpoint->mss);
}

size_t mam_endpoint_serialized_size(mam_endpoint_t const *const endpoint) {
  size_t mss_size = mss_stored_size(&endpoint->mss);

  return pb3_sizeof_ntrytes(MAM2_ENDPOINT_ID_SIZE / 3) +
         pb3_sizeof_size_t(trits_size(endpoint->name)) +
         pb3_sizeof_ntrytes(trits_size(endpoint->name) / 3) +
         pb3_sizeof_size_t(mss_size) + pb3_sizeof_ntrytes(mss_size / 3);
}

retcode_t mam_endpoint_serialize(mam_endpoint_t const *const endpoint,
                                 trits_t *const buffer) {
  size_t mss_size = mss_stored_size(&endpoint->mss);

  pb3_encode_ntrytes(trits_from_rep(MAM2_ENDPOINT_ID_SIZE, endpoint->id),
                     buffer);
  pb3_encode_size_t(trits_size(endpoint->name), buffer);
  pb3_encode_ntrytes(endpoint->name, buffer);
  pb3_encode_size_t(mss_size, buffer);
  mss_save(&endpoint->mss, trits_take(*buffer, mss_size));
  trits_advance(buffer, mss_size);

  return RC_OK;
}

retcode_t mam_endpoint_deserialize(trits_t *const buffer,
                                   mam_endpoint_t *const endpoint) {
  retcode_t ret = RC_OK;
  size_t size = 0;
  mss_mt_height_t height = 0;

  fprintf(stderr, "DESERIALIZE\n");

  //   TODO err_guard(trits_size(*b) >= ?, err_eof);

  if ((ret = pb3_decode_ntrytes(
           trits_from_rep(MAM2_ENDPOINT_ID_SIZE, endpoint->id), buffer)) !=
      RC_OK) {
    return ret;
  }

  if ((ret = pb3_decode_size_t(&size, buffer)) != RC_OK) {
    return ret;
  }
  if (trits_is_null(endpoint->name = trits_alloc(size))) {
    return RC_OOM;
  }
  if ((ret = pb3_decode_ntrytes(endpoint->name, buffer)) != RC_OK) {
    return ret;
  }

  if ((ret = pb3_decode_size_t(&size, buffer)) != RC_OK) {
    return ret;
  }

  fprintf(stderr, "MSS SIZE %d\n", size);

  MAM2_TRITS_DEF0(ts, MAM2_MSS_SKN_SIZE);
  ts = MAM2_TRITS_INIT(ts, MAM2_MSS_SKN_SIZE);
  trits_set_zero(ts);

  trits_copy(trits_take(*buffer, 4), trits_take(ts, 4));
  height = trits_get6(ts);

  fprintf(stderr, "HEIGHT %d\n", height);

  if ((ret = mss_create(&endpoint->mss, height)) != RC_OK) {
    return ret;
  }

  // TODO: need to pass prng, sponge, wots somehow
  // mss_init(ep->mss, prng, sponge, wots, d, nonce1, nonce2);

  // TODO: update mss_save interface
  // if ((ret = mss_load(&endpoint->mss, buffer)) != RC_OK) {
  //   return ret;
  // }

  trits_advance(buffer, size);

  return ret;
}

size_t mam_endpoints_serialized_size(mam_endpoint_t_set_t const endpoints) {
  mam_endpoint_t_set_entry_t *entry = NULL;
  mam_endpoint_t_set_entry_t *tmp = NULL;
  size_t size = pb3_sizeof_size_t(mam_endpoint_t_set_size(endpoints));

  HASH_ITER(hh, endpoints, entry, tmp) {
    size += mam_endpoint_serialized_size(&entry->value);
  }

  return size;
}

retcode_t mam_endpoints_serialize(mam_endpoint_t_set_t const endpoints,
                                  trits_t *const buffer) {
  retcode_t ret = RC_OK;
  mam_endpoint_t_set_entry_t *entry = NULL;
  mam_endpoint_t_set_entry_t *tmp = NULL;

  pb3_encode_size_t(mam_endpoint_t_set_size(endpoints), buffer);

  HASH_ITER(hh, endpoints, entry, tmp) {
    if ((ret = mam_endpoint_serialize(&entry->value, buffer)) != RC_OK) {
      return ret;
    }
  }

  return ret;
}

retcode_t mam_endpoints_deserialize(trits_t *const buffer,
                                    mam_endpoint_t_set_t *const endpoints) {
  retcode_t ret = RC_OK;
  mam_endpoint_t endpoint;
  size_t size = 0;

  if ((ret = pb3_decode_size_t(&size, buffer)) != RC_OK) {
    return ret;
  }

  while (size--) {
    if ((ret = mam_endpoint_deserialize(buffer, &endpoint)) != RC_OK) {
      return ret;
    }
    if ((ret = mam_endpoint_t_set_add(endpoints, &endpoint)) != RC_OK) {
      return ret;
    }
  }

  return ret;
}
